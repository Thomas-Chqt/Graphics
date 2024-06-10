/*
 * ---------------------------------------------------
 * MetalGraphicAPI.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/19 20:32:51
 * ---------------------------------------------------
 */

#include "GraphicAPI/Metal/MetalGraphicAPI.hpp"

#include "GraphicAPI/Metal/MetalFrameBuffer.hpp"
#include "GraphicAPI/Metal/MetalIndexBuffer.hpp"
#include "GraphicAPI/Metal/MetalTexture.hpp"
#include "Graphics/FrameBuffer.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "GraphicAPI/Metal/MetalGraphicPipeline.hpp"
#include "Graphics/IndexBuffer.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/ShaderLibrary.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/String.hpp"
#include <Foundation/NSString.h>
#include <Foundation/NSURL.h>
#include "GraphicAPI/Metal/MetalVertexBuffer.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/Types.hpp"
#include "UtilsCPP/UniquePtr.hpp"
#include "Window/MetalWindow.hpp"
#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>
#include <cassert>
#include <cstddef>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_metal.h"
#include "Graphics/Error.hpp"

using utils::SharedPtr;
using utils::UniquePtr;
using utils::String;
using utils::uint32;
using utils::Array;

namespace gfx
{

SharedPtr<GraphicAPI> Platform::newMetalGraphicAPI(const SharedPtr<Window>& renderTarget) const
{
    return SharedPtr<GraphicAPI>(new MetalGraphicAPI(renderTarget));
}

#ifdef GFX_IMGUI_ENABLED
void MetalGraphicAPI::useForImGui(ImGuiConfigFlags flags)
{
    assert(s_imguiEnabledAPI == nullptr && "Im gui is already using a graphic api object");
    
    ImGui::CreateContext();
    
    ImGui::GetIO().ConfigFlags = flags;

    m_window->imGuiInit();
    ImGui_ImplMetal_Init(m_mtlDevice);
    s_imguiEnabledAPI = this;
}
#endif

utils::SharedPtr<VertexBuffer> MetalGraphicAPI::newVertexBuffer(void* data, utils::uint64 size, const VertexBuffer::LayoutBase& layout) const
{
    (void)layout;
    return SharedPtr<VertexBuffer>(new MetalVertexBuffer(m_mtlDevice, data, size));
}

SharedPtr<GraphicPipeline> MetalGraphicAPI::newGraphicsPipeline(const String& vertexShaderName, const String& fragmentShaderName) const { @autoreleasepool
{
    return SharedPtr<GraphicPipeline>(new MetalGraphicPipeline(m_mtlDevice, vertexShaderName, fragmentShaderName));
}}

SharedPtr<IndexBuffer> MetalGraphicAPI::newIndexBuffer(const Array<uint32>& indices) const
{
    return SharedPtr<IndexBuffer>(new MetalIndexBuffer(m_mtlDevice, indices));
}

SharedPtr<Texture> MetalGraphicAPI::newTexture(uint32 width, uint32 height, Texture::PixelFormat pxFormat) const { @autoreleasepool
{
    MTLTextureDescriptor* textureDescriptor = [[[MTLTextureDescriptor alloc] init] autorelease];

    textureDescriptor.width = width;
    textureDescriptor.height = height;
    switch (pxFormat)
    {
    case Texture::PixelFormat::RGBA:
        textureDescriptor.pixelFormat = MTLPixelFormatRGBA8Unorm;
        break;
    case Texture::PixelFormat::ARGB:
        textureDescriptor.pixelFormat = MTLPixelFormatBGRA8Unorm;
        break;
    }

    return SharedPtr<Texture>(new MetalTexture(m_mtlDevice, textureDescriptor));
}}

utils::SharedPtr<FrameBuffer> MetalGraphicAPI::newFrameBuffer(utils::uint32 width, utils::uint32 height) const { @autoreleasepool
{
    MTLTextureDescriptor* colorTextureDescriptor = [[[MTLTextureDescriptor alloc] init] autorelease];
    colorTextureDescriptor.textureType = MTLTextureType2D;
    colorTextureDescriptor.width = width;
    colorTextureDescriptor.height = height;
    colorTextureDescriptor.pixelFormat = MTLPixelFormatRGBA8Unorm;
    colorTextureDescriptor.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;

    return SharedPtr<FrameBuffer>(new MetalFrameBuffer(MetalTexture(m_mtlDevice, colorTextureDescriptor)));
}}

void MetalGraphicAPI::beginFrame(const RenderPassDescriptor& renderPassDescriptor)
{
    m_commandBuffer = [[m_commandQueue commandBuffer] retain];
    assert(m_commandBuffer);

    m_currentDrawable = [[m_window->metalLayer() nextDrawable] retain];
    assert(m_currentDrawable);

    #ifdef GFX_IMGUI_ENABLED
    if (s_imguiEnabledAPI == this)
    {
        MTLRenderPassDescriptor* renderPassDescriptor = [[[MTLRenderPassDescriptor alloc] init] autorelease];
    
        renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionLoad;
        renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
        renderPassDescriptor.colorAttachments[0].texture = m_currentDrawable.texture;

        ImGui_ImplMetal_NewFrame(renderPassDescriptor);
        m_window->imGuiNewFrame();
        ImGui::NewFrame();
    }
    #endif

    beginRenderPass(renderPassDescriptor);
}

void MetalGraphicAPI::useGraphicsPipeline(const utils::SharedPtr<GraphicPipeline>& graphicsPipeline) { @autoreleasepool
{
    if (utils::SharedPtr<MetalGraphicPipeline> mtlGraphicsPipeline = graphicsPipeline.dynamicCast<MetalGraphicPipeline>())
    {
        MTLRenderPipelineDescriptor* renderPipelineDescriptor = [[[MTLRenderPipelineDescriptor alloc] init] autorelease];
        renderPipelineDescriptor.colorAttachments[0].pixelFormat = (MTLPixelFormat)m_renderPassTargetPixelFormat;
        renderPipelineDescriptor.colorAttachments[0].blendingEnabled = YES;

        renderPipelineDescriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
        renderPipelineDescriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;

        renderPipelineDescriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
        renderPipelineDescriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;

        renderPipelineDescriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
        renderPipelineDescriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;

        [m_commandEncoder setRenderPipelineState:mtlGraphicsPipeline->makeRenderPipelineState(m_mtlDevice, renderPipelineDescriptor)];
        m_renderPassObjects.append(UniquePtr<utils::SharedPtrBase>(new SharedPtr<GraphicPipeline>(graphicsPipeline)));
    }
    else
        throw utils::RuntimeError("GraphicPipeline is not MetalGraphicPipeline");
}}

void MetalGraphicAPI::useVertexBuffer(const utils::SharedPtr<VertexBuffer>& vertexBuffer) { @autoreleasepool
{
    if (utils::SharedPtr<MetalVertexBuffer> mtlVertexBuffer = vertexBuffer.dynamicCast<MetalVertexBuffer>())
    {
        [m_commandEncoder setVertexBuffer:mtlVertexBuffer->mtlBuffer() offset:0 atIndex:0];
        m_renderPassObjects.append(UniquePtr<utils::SharedPtrBase>(new SharedPtr<VertexBuffer>(vertexBuffer)));
    }
    else
        throw utils::RuntimeError("VertexBuffer is not MetalVertexBuffer");
}}

void MetalGraphicAPI::setVertexUniform(utils::uint32 index, const math::vec4f& vec) { @autoreleasepool
{
    [m_commandEncoder setVertexBytes:(const void *)&vec length:sizeof(math::vec4f) atIndex:index];
}}

void MetalGraphicAPI::setVertexUniform(utils::uint32 index, const math::mat4x4& mat) { @autoreleasepool
{
    [m_commandEncoder setVertexBytes:(const void *)&mat length:sizeof(math::mat4x4) atIndex:index];
}}

void MetalGraphicAPI::setVertexUniform(utils::uint32 index, const math::vec2f& vec){ @autoreleasepool
{
    [m_commandEncoder setVertexBytes:(const void *)&vec length:sizeof(math::vec2f) atIndex:index];
}}

void MetalGraphicAPI::setVertexUniform(utils::uint32 index, const math::mat3x3& mat) { @autoreleasepool
{
    [m_commandEncoder setVertexBytes:(const void *)&mat length:sizeof(math::mat3x3) atIndex:index];
}}

void MetalGraphicAPI::setFragmentUniform(utils::uint32 index, const math::vec4f& vec) { @autoreleasepool
{
    [m_commandEncoder setFragmentBytes:(const void *)&vec length:sizeof(math::vec4f) atIndex:index];
}}

void MetalGraphicAPI::setFragmentTexture(utils::uint32 index, const utils::SharedPtr<Texture>& texture) { @autoreleasepool
{
    if (utils::SharedPtr<MetalTexture> mtlTexture = texture.dynamicCast<MetalTexture>())
    {
        [m_commandEncoder setFragmentTexture:mtlTexture->mtlTexture() atIndex:index];
        m_renderPassObjects.append(UniquePtr<utils::SharedPtrBase>(new SharedPtr<Texture>(texture)));
    }
    else
        throw utils::RuntimeError("Texture is not MetalTexture");
}}

void MetalGraphicAPI::setFragmentTexture(utils::uint32 index, const utils::SharedPtr<FrameBuffer>& frameBuffer) { @autoreleasepool
{
    if (utils::SharedPtr<MetalFrameBuffer> mtlFrameBuffer = frameBuffer.dynamicCast<MetalFrameBuffer>())
    {
        [m_commandEncoder setFragmentTexture:mtlFrameBuffer->colorTexture().mtlTexture() atIndex:index];
        m_renderPassObjects.append(UniquePtr<utils::SharedPtrBase>(new SharedPtr<FrameBuffer>(frameBuffer)));
    }
    else
        throw utils::RuntimeError("FrameBuffer is not MetalFrameBuffer");
}}

void MetalGraphicAPI::drawVertices(utils::uint32 start, utils::uint32 count) { @autoreleasepool
{
    [m_commandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:start vertexCount:count];
}}

void MetalGraphicAPI::drawIndexedVertices(const utils::SharedPtr<IndexBuffer>& indexBuffer) { @autoreleasepool
{
    if (utils::SharedPtr<MetalIndexBuffer> mtlIndexBuffer = indexBuffer.dynamicCast<MetalIndexBuffer>())
    {
        [m_commandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                     indexCount:mtlIndexBuffer->indexCount()
                                      indexType:MTLIndexTypeUInt32
                                    indexBuffer:mtlIndexBuffer->mtlBuffer()
                              indexBufferOffset:0];
        m_renderPassObjects.append(UniquePtr<utils::SharedPtrBase>(new SharedPtr<IndexBuffer>(indexBuffer)));
    }
    else
        throw utils::RuntimeError("IndexBuffer is not MetalIndexBuffer");
}}

void MetalGraphicAPI::nextRenderPass(const RenderPassDescriptor& renderPassDescriptor)
{
    endRenderPass();
    beginRenderPass(renderPassDescriptor);
}

void MetalGraphicAPI::endFrame() { @autoreleasepool
{
    #ifdef GFX_IMGUI_ENABLED
    if (s_imguiEnabledAPI == this)
    {
        ImGui::Render();
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), m_commandBuffer, m_commandEncoder);

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }
    #endif

    endRenderPass();
    
    [m_commandBuffer presentDrawable:m_currentDrawable];
    [m_commandBuffer commit];

    [m_currentDrawable release];
    [m_commandBuffer release];
}}

MetalGraphicAPI::~MetalGraphicAPI() { @autoreleasepool
{
    #ifdef GFX_IMGUI_ENABLED
    if (s_imguiEnabledAPI == this)
    {
        ImGui_ImplMetal_Shutdown();
        m_window->imGuiShutdown();
        ImGui::DestroyContext();
        s_imguiEnabledAPI = nullptr;
    }
    #endif

    [m_commandQueue release];
    [m_mtlDevice release];
}}

MetalGraphicAPI::MetalGraphicAPI(const SharedPtr<Window>& renderTarget)
{
    if (SharedPtr<MetalWindow> mtlWindow = renderTarget.dynamicCast<MetalWindow>())
        m_window = mtlWindow;
    else
        throw utils::RuntimeError("Window is not MetalWindow");

    m_mtlDevice = MTLCreateSystemDefaultDevice();
    assert(m_mtlDevice);
    
    m_window->metalLayer().device = m_mtlDevice;

    m_commandQueue = [m_mtlDevice newCommandQueue];
    assert(m_commandQueue);
}

void MetalGraphicAPI::beginRenderPass(const RenderPassDescriptor& desc) { @autoreleasepool
{
    MTLRenderPassDescriptor* renderPassDescriptor = [[[MTLRenderPassDescriptor alloc] init] autorelease];
    
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(desc.clearColor.r, desc.clearColor.g, desc.clearColor.b, desc.clearColor.a);
    renderPassDescriptor.colorAttachments[0].loadAction = desc.clearBuffer ? MTLLoadActionClear : MTLLoadActionLoad;
    renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;

    if (desc.frameBuffer)
    {
        if (utils::SharedPtr<MetalFrameBuffer> mtlFrameBuffer = desc.frameBuffer.dynamicCast<MetalFrameBuffer>())
        {
            renderPassDescriptor.colorAttachments[0].texture = mtlFrameBuffer->colorTexture().mtlTexture();
            m_renderPassTargetPixelFormat = mtlFrameBuffer->colorTexture().mtlTexture().pixelFormat;
            m_renderPassObjects.append(UniquePtr<utils::SharedPtrBase>(new SharedPtr<FrameBuffer>(desc.frameBuffer)));
        }
        else
            throw utils::RuntimeError("FrameBuffer is not MetalFrameBuffer");
    }
    else
    {
        renderPassDescriptor.colorAttachments[0].texture = m_currentDrawable.texture;
        m_renderPassTargetPixelFormat = m_currentDrawable.texture.pixelFormat;
    }

    m_commandEncoder = [[m_commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor] retain];
    if (!m_commandEncoder)
        throw RenderCommandEncoderCreationError();
}}

void MetalGraphicAPI::endRenderPass() { @autoreleasepool
{
    [m_commandEncoder endEncoding];

    m_renderPassObjects.clear();
    [m_commandEncoder release];
}}

}