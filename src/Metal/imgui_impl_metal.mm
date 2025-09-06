/*
 * ---------------------------------------------------
 * imgui_impl_metal.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/30 14:21:30
 * ---------------------------------------------------
 */

#include "Graphics/Enums.hpp"
#include "Graphics/ShaderLib.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/VertexLayout.hpp"
#include "Metal/MetalBuffer.hpp"
#include "Metal/MetalCommandBuffer.hpp"
#include "Metal/MetalDevice.hpp"
#include "Metal/MetalEnums.h"
#include "Metal/imgui_impl_metal.hpp"
#include "Metal/MetalSampler.hpp"
#include "Metal/MetalTexture.hpp"
#include "Metal/MetalGraphicsPipeline.hpp"

namespace gfx
{

namespace
{
    inline CGSize MakeScaledSize(CGSize size, CGFloat scale) { return CGSizeMake(size.width * scale, size.height * scale); }
    inline double GetMachAbsoluteTimeInSeconds() { return static_cast<double>(clock_gettime_nsec_np(CLOCK_UPTIME_RAW)) / 1e9; }
}

struct FramebufferDescriptor
{
    uint32_t sampleCount = 1;
    ext::vector<PixelFormat> colorPixelFormats;
    ext::optional<PixelFormat> depthPixelFormat;
    ext::optional<PixelFormat> stencilPixelFormat;

    // Comparison operators to allow use as a key in ordered containers (e.g., std::map)
    friend bool operator==(const FramebufferDescriptor& a, const FramebufferDescriptor& b)
    {
        if (a.sampleCount != b.sampleCount)
            return false;

        auto na = a.colorPixelFormats.size();
        auto nb = b.colorPixelFormats.size();
        if (na != nb)
            return false;
        for (decltype(na) i = 0; i < na; ++i)
        {
            if (a.colorPixelFormats[i] != b.colorPixelFormats[i])
                return false;
        }

        const bool aHasDepth = a.depthPixelFormat.has_value();
        const bool bHasDepth = b.depthPixelFormat.has_value();
        if (aHasDepth != bHasDepth)
            return false;
        if (aHasDepth && (*a.depthPixelFormat != *b.depthPixelFormat))
            return false;

        const bool aHasStencil = a.stencilPixelFormat.has_value();
        const bool bHasStencil = b.stencilPixelFormat.has_value();
        if (aHasStencil != bHasStencil)
            return false;
        if (aHasStencil && (*a.stencilPixelFormat != *b.stencilPixelFormat))
            return false;

        return true;
    }

    friend bool operator<(const FramebufferDescriptor& a, const FramebufferDescriptor& b)
    {
        if (a.sampleCount != b.sampleCount)
            return a.sampleCount < b.sampleCount;

        auto na = a.colorPixelFormats.size();
        auto nb = b.colorPixelFormats.size();
        if (na != nb)
            return na < nb;
        for (decltype(na) i = 0; i < na; ++i)
        {
            if (a.colorPixelFormats[i] != b.colorPixelFormats[i])
                return a.colorPixelFormats[i] < b.colorPixelFormats[i];
        }

        const bool aHasDepth = a.depthPixelFormat.has_value();
        const bool bHasDepth = b.depthPixelFormat.has_value();
        if (aHasDepth != bHasDepth)
            return !aHasDepth && bHasDepth; // no depth < has depth
        if (aHasDepth && (*a.depthPixelFormat != *b.depthPixelFormat))
            return *a.depthPixelFormat < *b.depthPixelFormat;

        const bool aHasStencil = a.stencilPixelFormat.has_value();
        const bool bHasStencil = b.stencilPixelFormat.has_value();
        if (aHasStencil != bHasStencil)
            return !aHasStencil && bHasStencil; // no stencil < has stencil
        if (aHasStencil && (*a.stencilPixelFormat != *b.stencilPixelFormat))
            return *a.stencilPixelFormat < *b.stencilPixelFormat;

        return false; // equal
    }
};

struct ImGui_ImplMetal_Data
{
    const MetalDevice* device = nullptr;
    FramebufferDescriptor framebufferDescriptor;

    ext::map<FramebufferDescriptor, MetalGraphicsPipeline> graphicsPipelineCache;
    ext::vector<ext::unique_ptr<MetalBuffer>> bufferCache;
    ext::map<MetalBuffer*, double> bufferLastReuseTimes;
    double lastBufferCachePurge = GetMachAbsoluteTimeInSeconds();
    ext::mutex bufferCacheMtx;
    MetalSampler linearSampler;

    ext::unique_ptr<MetalBuffer> bufferOfLenght(uint32_t len)
    {
        double now = GetMachAbsoluteTimeInSeconds();

        {
            ext::scoped_lock<ext::mutex> lock(this->bufferCacheMtx);

            // Purge old buffers that haven't been useful for a while
            if (now - this->lastBufferCachePurge > 1.0)
            {
                auto res = this->bufferCache | ext::views::filter([&](auto& buff) -> bool {
                    return this->bufferLastReuseTimes[buff.get()] <= this->lastBufferCachePurge;
                });
                ext::vector<ext::unique_ptr<MetalBuffer>> survivors;
                ext::map<MetalBuffer*, double> survivorsLastReuseTimes;
                for (auto& buff : res) {
                    survivors.push_back(ext::move(buff));
                    survivorsLastReuseTimes[buff.get()] = this->bufferLastReuseTimes[buff.get()];
                }
                this->bufferCache = ext::move(survivors);
                this->bufferLastReuseTimes = ext::move(survivorsLastReuseTimes);
                this->lastBufferCachePurge = now;
            }

            // See if we have a buffer we can reuse
            auto bestCandidate = this->bufferCache.end();
            for (auto candidate = this->bufferCache.begin(); candidate != this->bufferCache.end(); ++candidate) {
                if ((*candidate)->size() >= len && (bestCandidate == this->bufferCache.end() || this->bufferLastReuseTimes[bestCandidate->get()] > this->bufferLastReuseTimes[candidate->get()]))
                    bestCandidate = candidate;
            }

            if (bestCandidate != this->bufferCache.end())
            {
                ext::unique_ptr<MetalBuffer> buff = ext::move(*bestCandidate);
                this->bufferCache.erase(bestCandidate);
                this->bufferLastReuseTimes[buff.get()] = now;
                return buff;
            }
        }

        // No luck; make a new buffer
        Buffer::Descriptor bufferDescriptor = {
            .size = len,
            .usages = BufferUsage::vertexBuffer,
            .storageMode = ResourceStorageMode::hostVisible
        };
        return ext::make_unique<MetalBuffer>(device, bufferDescriptor);
    }
};

struct ImGuiViewportDataMetal
{
    CAMetalLayer* MetalLayer = nullptr;
    id<MTLCommandQueue> CommandQueue = nil;
    MTLRenderPassDescriptor* RenderPassDescriptor = nullptr;
    NSWindow* Handle = nullptr;
    bool FirstFrame = true;
};

namespace
{

inline ImGui_ImplMetal_Data* ImGui_ImplMetal_GetBackendData() { return GetCurrentContext() ? static_cast<ImGui_ImplMetal_Data*>(GetIO().BackendRendererUserData) : nullptr; }

void ImGui_ImplMetal_CreateWindow(ImGuiViewport*);
void ImGui_ImplMetal_DestroyWindow(ImGuiViewport*);
void ImGui_ImplMetal_SetWindowSize(ImGuiViewport*, ImVec2 size);
void ImGui_ImplMetal_RenderWindow(ImGuiViewport*, void*);

void ImGui_ImplMetal_DestroyTexture(ImTextureData* tex);
void ImGui_ImplMetal_CreateDeviceObjects();
MetalGraphicsPipeline graphicPipelineStateForFramebufferDescriptor(const MetalDevice&, const FramebufferDescriptor&);
void ImGui_ImplMetal_UpdateTexture(ImTextureData*);
void ImGui_ImplMetal_SetupRenderState(ImDrawData* draw_data, id<MTLCommandBuffer>, id<MTLRenderCommandEncoder>, const MetalGraphicsPipeline&, const MetalBuffer& vertexBuffer, size_t vertexBufferOffset);

}

void ImGui_ImplMetal_Init(const MetalDevice* device, const ext::vector<PixelFormat>& colorPixelFomats, const ext::optional<PixelFormat>& depthPixelFormat) { @autoreleasepool
{
    ImGuiIO& io = GetIO();
    IMGUI_CHECKVERSION();
    IM_ASSERT(io.BackendRendererUserData == nullptr && "Already initialized a renderer backend!");

    auto* bd = new ImGui_ImplMetal_Data; // NOLINT(cppcoreguidelines-owning-memory)
    bd->device = device;
    bd->framebufferDescriptor = {
        .sampleCount = 1,
        .colorPixelFormats = colorPixelFomats,
        .depthPixelFormat = depthPixelFormat,
        .stencilPixelFormat = std::nullopt
    };
    bd->linearSampler = MetalSampler(*device, Sampler::Descriptor{});

    io.BackendRendererUserData = (void*)bd;
    io.BackendRendererName = "imgui_impl_metal";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset; // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
    io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;  // We can honor ImGuiPlatformIO::Textures[] requests during render.
    io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports; // We can create multi-viewports on the Renderer side (optional)

    ImGuiPlatformIO& platform_io = GetPlatformIO();
    platform_io.Renderer_CreateWindow = ImGui_ImplMetal_CreateWindow;
    platform_io.Renderer_DestroyWindow = ImGui_ImplMetal_DestroyWindow;
    platform_io.Renderer_SetWindowSize = ImGui_ImplMetal_SetWindowSize;
    platform_io.Renderer_RenderWindow = ImGui_ImplMetal_RenderWindow;
}}

void ImGui_ImplMetal_Shutdown() { @autoreleasepool
{
    ImGui_ImplMetal_Data* bd = ImGui_ImplMetal_GetBackendData();
    IM_UNUSED(bd);
    IM_ASSERT(bd != nullptr && "No renderer backend to shutdown, or already shutdown?");

    // ImGui_ImplMetal_ShutdownMultiViewportSupport
    DestroyPlatformWindows();
    //

    // ImGui_ImplMetal_DestroyDeviceObjects
    // {
    //     ImGui_ImplMetal_Data* bd = ImGui_ImplMetal_GetBackendData();
    //
    // Destroy all textures
    for (ImTextureData* tex : GetPlatformIO().Textures) {
        if (tex->RefCount == 1)
            ImGui_ImplMetal_DestroyTexture(tex);
    }

    // ImGui_ImplMetal_InvalidateDeviceObjectsForPlatformWindows
    // {
    ImGuiPlatformIO& platform_io = GetPlatformIO();
    for (int i = 1; i < platform_io.Viewports.Size; i++)
        if (platform_io.Viewports[i]->RendererUserData)
            ImGui_ImplMetal_DestroyWindow(platform_io.Viewports[i]);
    // } ImGui_ImplMetal_InvalidateDeviceObjectsForPlatformWindows
    bd->graphicsPipelineCache.clear();
    // } // ImGui_ImplMetal_DestroyDeviceObjects

    //ImGui_ImplMetal_DestroyBackendData();
    delete bd; // NOLINT(cppcoreguidelines-owning-memory)

    ImGuiIO& io = GetIO();
    io.BackendRendererName = nullptr;
    io.BackendRendererUserData = nullptr;
    io.BackendFlags &= ~(ImGuiBackendFlags_RendererHasVtxOffset | ImGuiBackendFlags_RendererHasTextures | ImGuiBackendFlags_RendererHasViewports);
}}

void ImGui_ImplMetal_NewFrame() { @autoreleasepool
{
    ImGui_ImplMetal_Data* bd = ImGui_ImplMetal_GetBackendData();
    IM_ASSERT(bd != nil && "Context or backend not initialized! Did you call ImGui_ImplMetal_Init()?");
    if (bd->graphicsPipelineCache.find(bd->framebufferDescriptor) == bd->graphicsPipelineCache.end())
        ImGui_ImplMetal_CreateDeviceObjects();
}}

void ImGui_ImplMetal_RenderDrawData(ImDrawData* draw_data, id<MTLCommandBuffer> commandBuffer, id<MTLRenderCommandEncoder> commandEncoder) { @autoreleasepool
{
    ImGui_ImplMetal_Data* bd = ImGui_ImplMetal_GetBackendData();

    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0 || draw_data->CmdLists.Size == 0)
        return;

    // Catch up with texture updates. Most of the times, the list will have 1 element with an OK status, aka nothing to do.
    // (This almost always points to ImGui::GetPlatformIO().Textures[] but is part of ImDrawData to allow overriding or disabling texture updates).
    if (draw_data->Textures != nullptr)
        for (ImTextureData* tex : *draw_data->Textures)
            if (tex->Status != ImTextureStatus_OK)
                ImGui_ImplMetal_UpdateTexture(tex);

    // Try to retrieve a render pipeline state that is compatible with the framebuffer config for this frame
    // The hit rate for this cache should be very near 100%.
    auto graphicsPipeline = bd->graphicsPipelineCache.find(bd->framebufferDescriptor);
    if (graphicsPipeline == bd->graphicsPipelineCache.end())
    {
        // No luck; make a new render pipeline state
        // Cache render pipeline state for later reuse
        auto [it, res] = bd->graphicsPipelineCache.insert(ext::make_pair(bd->framebufferDescriptor, graphicPipelineStateForFramebufferDescriptor(*bd->device, bd->framebufferDescriptor)));
        graphicsPipeline = it;
    }

    size_t vertexBufferLength = (size_t)draw_data->TotalVtxCount * sizeof(ImDrawVert);
    size_t indexBufferLength = (size_t)draw_data->TotalIdxCount * sizeof(ImDrawIdx);
    ext::unique_ptr<MetalBuffer> vertexBuffer = bd->bufferOfLenght(vertexBufferLength);
    ext::unique_ptr<MetalBuffer> indexBuffer = bd->bufferOfLenght(indexBufferLength);

    ImGui_ImplMetal_SetupRenderState(draw_data, commandBuffer, commandEncoder, graphicsPipeline->second, *vertexBuffer, 0);

    // Will project scissor/clipping rectangles into framebuffer space
    ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    // Render command lists
    size_t vertexBufferOffset = 0;
    size_t indexBufferOffset = 0;
    for (const ImDrawList* draw_list : draw_data->CmdLists)
    {
        memcpy(vertexBuffer->content<char>() + vertexBufferOffset, draw_list->VtxBuffer.Data, (size_t)draw_list->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(indexBuffer->content<char>() + indexBufferOffset, draw_list->IdxBuffer.Data, (size_t)draw_list->IdxBuffer.Size * sizeof(ImDrawIdx));

        for (int cmd_i = 0; cmd_i < draw_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &draw_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    ImGui_ImplMetal_SetupRenderState(draw_data, commandBuffer, commandEncoder, graphicsPipeline->second, *vertexBuffer, vertexBufferOffset);
                else
                    pcmd->UserCallback(draw_list, pcmd);
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x, (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
                ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x, (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);

                // Clamp to viewport as setScissorRect() won't accept values that are off bounds
                if (clip_min.x < 0.0f)
                {
                    clip_min.x = 0.0f;
                }
                if (clip_min.y < 0.0f)
                {
                    clip_min.y = 0.0f;
                }
                if (clip_max.x > (float)fb_width)
                {
                    clip_max.x = (float)fb_width;
                }
                if (clip_max.y > (float)fb_height)
                {
                    clip_max.y = (float)fb_height;
                }
                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                    continue;
                if (pcmd->ElemCount == 0) // drawIndexedPrimitives() validation doesn't accept this
                    continue;

                // Apply scissor/clipping rectangle
                MTLScissorRect scissorRect =
                    {
                        .x = NSUInteger(clip_min.x),
                        .y = NSUInteger(clip_min.y),
                        .width = NSUInteger(clip_max.x - clip_min.x),
                        .height = NSUInteger(clip_max.y - clip_min.y)};
                [commandEncoder setScissorRect:scissorRect];

                // Bind texture, Draw
                if (ImTextureID tex_id = pcmd->GetTexID())
                    [commandEncoder setFragmentTexture:ext::bit_cast<id<MTLTexture>>(tex_id) atIndex:0];

                [commandEncoder setVertexBufferOffset:(vertexBufferOffset + pcmd->VtxOffset * sizeof(ImDrawVert)) atIndex:5];
                [commandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                                                                        indexCount:pcmd->ElemCount
                                                                                         indexType:sizeof(ImDrawIdx) == 2 ? MTLIndexTypeUInt16 : MTLIndexTypeUInt32
                                                                                       indexBuffer:indexBuffer->mtlBuffer()
                                                                                 indexBufferOffset:indexBufferOffset + pcmd->IdxOffset * sizeof(ImDrawIdx)];
            }
        }

        vertexBufferOffset += (size_t)draw_list->VtxBuffer.Size * sizeof(ImDrawVert);
        indexBufferOffset += (size_t)draw_list->IdxBuffer.Size * sizeof(ImDrawIdx);
    }

    auto* vertexBufferPtr = vertexBuffer.release();
    auto* indexBufferPtr = indexBuffer.release();

    [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer>) {
      dispatch_async(dispatch_get_main_queue(), ^{
        {
            ext::scoped_lock<ext::mutex> lock(bd->bufferCacheMtx);
            bd->bufferCache.push_back(ext::unique_ptr<MetalBuffer>(vertexBufferPtr));
            bd->bufferCache.push_back(ext::unique_ptr<MetalBuffer>(indexBufferPtr));
        }
      });
    }];
}}

namespace
{

void ImGui_ImplMetal_CreateWindow(ImGuiViewport* viewport) { @autoreleasepool
{
    ImGui_ImplMetal_Data* bd = ImGui_ImplMetal_GetBackendData();
    auto* data = new ImGuiViewportDataMetal; // NOLINT(cppcoreguidelines-owning-memory)

    // PlatformHandleRaw should always be a NSWindow*, whereas PlatformHandle might be a higher-level handle (e.g. GLFWWindow*, SDL_Window*).
    // Some back-ends will leave PlatformHandleRaw == 0, in which case we assume PlatformHandle will contain the NSWindow*.
    auto* window = static_cast<NSWindow*>(viewport->PlatformHandleRaw ? viewport->PlatformHandleRaw : viewport->PlatformHandle);
    IM_ASSERT(window != nullptr);

    CAMetalLayer* layer = [CAMetalLayer layer];
    layer.device = bd->device->mtlDevice();
    layer.framebufferOnly = YES;
    layer.pixelFormat = toMTLPixelFormat(bd->framebufferDescriptor.colorPixelFormats.front());

    NSView* view = window.contentView;
    view.layer = layer;
    view.wantsLayer = YES;
    data->MetalLayer = layer;
    data->CommandQueue = [bd->device->mtlDevice() newCommandQueue];
    data->RenderPassDescriptor = [[MTLRenderPassDescriptor alloc] init];
    data->Handle = [window retain];

    viewport->RendererUserData = data;
}}

void ImGui_ImplMetal_DestroyWindow(ImGuiViewport* viewport) { @autoreleasepool
{
    // The main viewport (owned by the application) will always have RendererUserData == 0 since we didn't create the data for it.
    if (auto* data = static_cast<ImGuiViewportDataMetal*>(viewport->RendererUserData)) {
        [data->Handle release];
        [data->RenderPassDescriptor release];
        [data->CommandQueue release];
        delete data; // NOLINT(cppcoreguidelines-owning-memory)
    }

    viewport->RendererUserData = nullptr;
}}

void ImGui_ImplMetal_SetWindowSize(ImGuiViewport* viewport, ImVec2 size) { @autoreleasepool
{
    auto* data = static_cast<ImGuiViewportDataMetal*>(viewport->RendererUserData);
    data->MetalLayer.drawableSize = MakeScaledSize(CGSizeMake(size.x, size.y), viewport->DpiScale);
}}

void ImGui_ImplMetal_RenderWindow(ImGuiViewport* viewport, void*) { @autoreleasepool
{
    auto* data = static_cast<ImGuiViewportDataMetal*>(viewport->RendererUserData);

    auto* window = static_cast<NSWindow*>(viewport->PlatformHandleRaw ? viewport->PlatformHandleRaw : viewport->PlatformHandle);
    IM_ASSERT(window != nullptr);

    // Always render the first frame, regardless of occlusionState, to avoid an initial flicker
    if ((window.occlusionState & NSWindowOcclusionStateVisible) == 0 && !data->FirstFrame)
    {
        // Do not render windows which are completely occluded. Calling -[CAMetalLayer nextDrawable] will hang for
        // approximately 1 second if the Metal layer is completely occluded.
        return;
    }
    data->FirstFrame = false;

    auto fb_scale = (float)window.backingScaleFactor;

    // https://github.com/ocornut/imgui/issues/8856
    // if (data->MetalLayer.contentsScale != fb_scale)
    // {
    data->MetalLayer.contentsScale = fb_scale;
    data->MetalLayer.drawableSize = MakeScaledSize(window.frame.size, fb_scale);
    // }

    id<CAMetalDrawable> drawable = [data->MetalLayer nextDrawable];
    if (drawable == nil)
        return;

    MTLRenderPassDescriptor* renderPassDescriptor = data->RenderPassDescriptor;
    renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 0);
    if ((viewport->Flags & ImGuiViewportFlags_NoRendererClear) == 0)
        renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;

    ImGui_ImplMetal_Data* bd = ImGui_ImplMetal_GetBackendData();

    FramebufferDescriptor oldFramebufferDescriptor = bd->framebufferDescriptor;

    bd->framebufferDescriptor = FramebufferDescriptor{
        .colorPixelFormats = { toPixelFormat(drawable.texture.pixelFormat) },
        .depthPixelFormat = ext::nullopt,
        .stencilPixelFormat = ext::nullopt
    };

    id<MTLCommandBuffer> commandBuffer = [data->CommandQueue commandBuffer];
    id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
    ImGui_ImplMetal_RenderDrawData(viewport->DrawData, commandBuffer, renderEncoder);
    [renderEncoder endEncoding];

    bd->framebufferDescriptor = oldFramebufferDescriptor;

    [commandBuffer presentDrawable:drawable];
    [commandBuffer commit];
}}

void ImGui_ImplMetal_DestroyTexture(ImTextureData* tex) { @autoreleasepool
{
    auto* backend_tex = static_cast<MetalTexture*>(tex->BackendUserData);
    if (backend_tex == nullptr)
        return;
    delete backend_tex; // NOLINT(cppcoreguidelines-owning-memory)

    // Clear identifiers and mark as destroyed (in order to allow e.g. calling InvalidateDeviceObjects while running)
    tex->SetTexID(ImTextureID_Invalid);
    tex->SetStatus(ImTextureStatus_Destroyed);
    tex->BackendUserData = nullptr;
}}

void ImGui_ImplMetal_CreateDeviceObjects() { @autoreleasepool
{
    ImGui_ImplMetal_Data* bd = ImGui_ImplMetal_GetBackendData();

    bd->graphicsPipelineCache.insert(ext::make_pair(bd->framebufferDescriptor, graphicPipelineStateForFramebufferDescriptor(*bd->device, bd->framebufferDescriptor)));
    // ImGui_ImplMetal_CreateDeviceObjectsForPlatformWindows
    // {
    ImGuiPlatformIO& platform_io = GetPlatformIO();
    for (int i = 1; i < platform_io.Viewports.Size; i++)
        if (!platform_io.Viewports[i]->RendererUserData)
            ImGui_ImplMetal_CreateWindow(platform_io.Viewports[i]);
    // }
}}

MetalGraphicsPipeline graphicPipelineStateForFramebufferDescriptor(const MetalDevice& device, const FramebufferDescriptor& framebufferDescriptor)
{
    ext::unique_ptr<ShaderLib> shaderLib = device.newShaderLib(GFX_SHADER_SLIB);

    GraphicsPipeline::Descriptor graphicsPipelineDescriptor = {
        .vertexLayout = VertexLayout{
            .stride = sizeof(ImDrawVert),
            .attributes = {
                VertexAttribute{.format=VertexAttributeFormat::float2, .offset=offsetof(ImDrawVert, pos) },
                VertexAttribute{.format=VertexAttributeFormat::float2, .offset=offsetof(ImDrawVert, uv)  },
                VertexAttribute{.format=VertexAttributeFormat::uint,   .offset=offsetof(ImDrawVert, col) },
            }
        },
        .vertexShader = &shaderLib->getFunction("vertex_main"),
        .fragmentShader = &shaderLib->getFunction("fragment_main"),
        .colorAttachmentPxFormats = framebufferDescriptor.colorPixelFormats,
        .depthAttachmentPxFormat = framebufferDescriptor.depthPixelFormat,
        .blendOperation = BlendOperation::srcA_plus_1_minus_srcA
    };

    return { device, graphicsPipelineDescriptor };
}

void ImGui_ImplMetal_UpdateTexture(ImTextureData* tex) { @autoreleasepool
{
    ImGui_ImplMetal_Data* bd = ImGui_ImplMetal_GetBackendData();
    if (tex->Status == ImTextureStatus_WantCreate)
    {
        // Create and upload new texture to graphics system
        // IMGUI_DEBUG_LOG("UpdateTexture #%03d: WantCreate %dx%d\n", tex->UniqueID, tex->Width, tex->Height);
        IM_ASSERT(tex->TexID == ImTextureID_Invalid && tex->BackendUserData == nullptr);
        IM_ASSERT(tex->Format == ImTextureFormat_RGBA32);

        // We are retrieving and uploading the font atlas as a 4-channels RGBA texture here.
        // In theory we could call GetTexDataAsAlpha8() and upload a 1-channel texture to save on memory access bandwidth.
        // However, using a shader designed for 1-channel texture would make it less obvious to use the ImTextureID facility to render users own textures.
        // You can make that change in your implementation.
        MTLTextureDescriptor* textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                                     width:(NSUInteger)tex->Width
                                                                                                    height:(NSUInteger)tex->Height
                                                                                                 mipmapped:NO];
        textureDescriptor.usage = MTLTextureUsageShaderRead;
#if TARGET_OS_OSX || TARGET_OS_MACCATALYST
        textureDescriptor.storageMode = MTLStorageModeManaged;
#else
        textureDescriptor.storageMode = MTLStorageModeShared;
#endif
        id<MTLTexture> texture = [bd->device->mtlDevice() newTextureWithDescriptor:textureDescriptor];
        [texture replaceRegion:MTLRegionMake2D(0, 0, (NSUInteger)tex->Width, (NSUInteger)tex->Height) mipmapLevel:0 withBytes:tex->Pixels bytesPerRow:(NSUInteger)tex->Width * 4];
        auto* backend_tex = new MetalTexture(bd->device, texture, Texture::Descriptor{
            .width=static_cast<uint32_t>(texture.width), .height=static_cast<uint32_t>(texture.height)});

        // Store identifiers
        tex->SetTexID(std::bit_cast<ImTextureID>(texture));
        tex->SetStatus(ImTextureStatus_OK);
        tex->BackendUserData = static_cast<void*>(backend_tex);
    }
    else if (tex->Status == ImTextureStatus_WantUpdates)
    {
        // Update selected blocks. We only ever write to textures regions which have never been used before!
        // This backend choose to use tex->Updates[] but you can use tex->UpdateRect to upload a single region.
        auto* backend_tex = static_cast<MetalTexture*>(tex->BackendUserData);
        for (ImTextureRect& r : tex->Updates)
        {
            [backend_tex->mtltexture() replaceRegion:MTLRegionMake2D((NSUInteger)r.x, (NSUInteger)r.y, (NSUInteger)r.w, (NSUInteger)r.h)
                                         mipmapLevel:0
                                           withBytes:tex->GetPixelsAt(r.x, r.y)
                                         bytesPerRow:(NSUInteger)tex->Width * 4];
        }
        tex->SetStatus(ImTextureStatus_OK);
    }
    else if (tex->Status == ImTextureStatus_WantDestroy && tex->UnusedFrames > 0)
    {
        ImGui_ImplMetal_DestroyTexture(tex);
    }
}}

void ImGui_ImplMetal_SetupRenderState(ImDrawData* draw_data, id<MTLCommandBuffer>, id<MTLRenderCommandEncoder> commandEncoder, const MetalGraphicsPipeline& graphicsPipeline, const MetalBuffer& vertexBuffer, size_t vertexBufferOffset)
{
    [commandEncoder setCullMode:MTLCullModeNone];
    if (graphicsPipeline.depthStencilState())
        [commandEncoder setDepthStencilState:graphicsPipeline.depthStencilState()];

    ImGui_ImplMetal_Data* bd = ImGui_ImplMetal_GetBackendData();
    
    [commandEncoder setFragmentSamplerState:bd->linearSampler.mtlSamplerState() atIndex:0];

    // Setup viewport, orthographic projection matrix
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to
    // draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayMin is typically (0,0) for single viewport apps.
    MTLViewport viewport = {
        .originX = 0.0,
        .originY = 0.0,
        .width = (double)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x),
        .height = (double)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y),
        .znear = 0.0,
        .zfar = 1.0};
    [commandEncoder setViewport:viewport];

    float L = draw_data->DisplayPos.x;
    float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
    float T = draw_data->DisplayPos.y;
    float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
    auto N = static_cast<float>(viewport.znear);
    auto F = static_cast<float>(viewport.zfar);
    const float ortho_projection[4][4] = // NOLINT(cppcoreguidelines-avoid-c-arrays)
        {
            {    2.0f / (R - L),              0.0f,        0.0f, 0.0f },
            {              0.0f,    2.0f / (T - B),        0.0f, 0.0f },
            {              0.0f,              0.0f, 1 / (F - N), 0.0f },
            { (R + L) / (L - R), (T + B) / (B - T), N / (F - N), 1.0f },
        };
    [commandEncoder setVertexBytes:&ortho_projection length:sizeof(ortho_projection) atIndex:0];

    [commandEncoder setRenderPipelineState:graphicsPipeline.renderPipelineState()];

    [commandEncoder setVertexBuffer:vertexBuffer.mtlBuffer() offset:0 atIndex:5];
    [commandEncoder setVertexBufferOffset:vertexBufferOffset atIndex:5];
}

}

}
