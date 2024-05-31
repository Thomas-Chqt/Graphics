/*
 * ---------------------------------------------------
 * MetalGraphicAPI.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/19 20:32:51
 * ---------------------------------------------------
 */

#include "GraphicAPI/Metal/MetalGraphicAPI.hpp"

#include "GraphicAPI/Metal/MetalIndexBuffer.hpp"
#include "GraphicAPI/Metal/MetalTexture.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "GraphicAPI/Metal/MetalGraphicPipeline.hpp"
#include "Graphics/IndexBuffer.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/ShaderLibrary.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/String.hpp"
#include <Foundation/Foundation.h>
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

using utils::SharedPtr;
using utils::UniquePtr;
using utils::String;
using utils::uint32;
using utils::Array;

#ifdef IMGUI_ENABLED
    class ImDrawData;
    class ImGuiContext;
    class ImFontAtlas;

    bool ImGui_ImplMetal_Init(id<MTLDevice> device);
    void ImGui_ImplMetal_Shutdown();
    void ImGui_ImplMetal_NewFrame(MTLRenderPassDescriptor*);
    void ImGui_ImplMetal_RenderDrawData(ImDrawData*, id<MTLCommandBuffer>, id<MTLRenderCommandEncoder>);
    
    namespace ImGui
    {
        ImDrawData* GetDrawData();
        ImGuiContext* CreateContext(ImFontAtlas* shared_font_atlas = NULL);
        void DestroyContext(ImGuiContext* ctx = NULL);
    }
#endif

namespace gfx
{

SharedPtr<GraphicAPI> Platform::newMetalGraphicAPI(const SharedPtr<Window>& renderTarget)
{
    return SharedPtr<GraphicAPI>(new MetalGraphicAPI(renderTarget));
}

void MetalGraphicAPI::setRenderTarget(const utils::SharedPtr<Window>& renderTarget) { @autoreleasepool
{
    if (SharedPtr<MetalWindow> mtlWindow = renderTarget.dynamicCast<MetalWindow>())
        m_renderTarget = mtlWindow;
    else
        throw utils::RuntimeError("Window is not MetalWindow");

    if (m_mtlDevice == nullptr)
    {
        m_mtlDevice = MTLCreateSystemDefaultDevice();
        assert(m_mtlDevice);
    }
    
    m_renderTarget->metalLayer().device = m_mtlDevice;

    if (m_commandQueue == nullptr)
    {
        m_commandQueue = [m_mtlDevice newCommandQueue];
        assert(m_commandQueue);
    }

    if (m_renderPassDescriptor == nullptr)
    {
        m_renderPassDescriptor = [[MTLRenderPassDescriptor alloc] init];
        assert(m_renderPassDescriptor);
        m_renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 1);
        m_renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
    }

    m_frameCount = 0;
}}

#ifdef IMGUI_ENABLED
void MetalGraphicAPI::useForImGui(const utils::Func<void()>& f)
{
    assert(s_imguiEnabledAPI == nullptr && "Im gui is already using a graphic api object");
    assert(m_renderTarget && "Render target need to be set before initializing imgui");
    
    ImGui::CreateContext();
    if (f) f();

    m_renderTarget->imGuiInit();
    ImGui_ImplMetal_Init(m_mtlDevice);
    s_imguiEnabledAPI = this;
}
#endif

void MetalGraphicAPI::setClearColor(const math::rgba& color)
{
    m_renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(color.r, color.g, color.b, color.a);
}

utils::SharedPtr<VertexBuffer> MetalGraphicAPI::newVertexBuffer(void* data, utils::uint64 size, const VertexBuffer::LayoutBase& layout)
{
    (void)layout;
    return SharedPtr<VertexBuffer>(new MetalVertexBuffer(m_mtlDevice, data, size));
}

SharedPtr<GraphicPipeline> MetalGraphicAPI::newGraphicsPipeline(const String& vertexShaderName, const String& fragmentShaderName, GraphicPipeline::BlendingOperation operation) { @autoreleasepool
{
    if (m_shaderLibrary == nil)
    {
        NSError *error;
        NSString* mtlShaderLibPath = [[[NSString alloc] initWithCString:ShaderLibrary::shared().getMetalShaderLibPath() encoding:NSUTF8StringEncoding] autorelease];
        m_shaderLibrary = [m_mtlDevice newLibraryWithURL:[NSURL URLWithString: mtlShaderLibPath] error:&error];
        assert(m_shaderLibrary);
    }

    return SharedPtr<GraphicPipeline>(new MetalGraphicPipeline(m_mtlDevice, m_shaderLibrary, m_renderTarget->metalLayer(), vertexShaderName, fragmentShaderName, operation));
}}

SharedPtr<IndexBuffer> MetalGraphicAPI::newIndexBuffer(const Array<uint32>& indices)
{
    return SharedPtr<IndexBuffer>(new MetalIndexBuffer(m_mtlDevice, indices));
}

SharedPtr<Texture> MetalGraphicAPI::newTexture(uint32 width, uint32 height, Texture::PixelFormat pxFormat) { @autoreleasepool
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

void MetalGraphicAPI::beginFrame(bool clearBuffer) { @autoreleasepool
{
    m_renderPassDescriptor.colorAttachments[0].loadAction = (m_frameCount < 3 || clearBuffer) ? MTLLoadActionClear : MTLLoadActionLoad;
    if (m_frameCount < 3)
        m_frameCount++;

    m_commandBuffer = [[m_commandQueue commandBuffer] retain];
    assert(m_commandBuffer);
	
	m_currentDrawable = [[m_renderTarget->metalLayer() nextDrawable] retain];
    assert(m_currentDrawable);

	m_renderPassDescriptor.colorAttachments[0].texture = m_currentDrawable.texture;

    m_commandEncoder = [[m_commandBuffer renderCommandEncoderWithDescriptor:m_renderPassDescriptor] retain];
    assert(m_commandEncoder);

    #ifdef IMGUI_ENABLED
    if (s_imguiEnabledAPI == this)
    {
        ImGui_ImplMetal_NewFrame(m_renderPassDescriptor);
        m_renderTarget->imGuiNewFrame();
    }
    #endif
}}

void MetalGraphicAPI::useGraphicsPipeline(const utils::SharedPtr<GraphicPipeline>& graphicsPipeline) { @autoreleasepool
{
    if (utils::SharedPtr<MetalGraphicPipeline> mtlGraphicsPipeline = graphicsPipeline.dynamicCast<MetalGraphicPipeline>())
    {
        [m_commandEncoder setRenderPipelineState:mtlGraphicsPipeline->renderPipelineState()];
        m_frameObjects.append(
            UniquePtr<utils::SharedPtrBase>(
                new SharedPtr<GraphicPipeline>(graphicsPipeline)
            )
        );
    }
    else
        throw utils::RuntimeError("GraphicPipeline is not MetalGraphicPipeline");
}}

void MetalGraphicAPI::useVertexBuffer(const utils::SharedPtr<VertexBuffer>& vertexBuffer) { @autoreleasepool
{
    if (utils::SharedPtr<MetalVertexBuffer> mtlVertexBuffer = vertexBuffer.dynamicCast<MetalVertexBuffer>())
    {
        [m_commandEncoder setVertexBuffer:mtlVertexBuffer->mtlBuffer() offset:0 atIndex:0];
        m_frameObjects.append(
            UniquePtr<utils::SharedPtrBase>(
                new SharedPtr<VertexBuffer>(vertexBuffer)
            )
        );
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
        m_frameObjects.append(
            UniquePtr<utils::SharedPtrBase>(
                new SharedPtr<Texture>(texture)
            )
        );
    }
    else
        throw utils::RuntimeError("Texture is not MetalTexture");
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
        m_frameObjects.append(
            UniquePtr<utils::SharedPtrBase>(
                new SharedPtr<IndexBuffer>(indexBuffer)
            )
        );
    }
    else
        throw utils::RuntimeError("IndexBuffer is not MetalIndexBuffer");
}}

void MetalGraphicAPI::endFrame() { @autoreleasepool
{
    #ifdef IMGUI_ENABLED
    if (s_imguiEnabledAPI == this)
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), m_commandBuffer, m_commandEncoder);
    #endif
    
    [m_commandEncoder endEncoding];

    [m_commandBuffer presentDrawable:m_currentDrawable];
    [m_commandBuffer commit];

    [m_commandEncoder release];
    [m_currentDrawable release];
    [m_commandBuffer release];
    m_frameObjects.clear();
}}

MetalGraphicAPI::~MetalGraphicAPI() { @autoreleasepool
{
    #ifdef IMGUI_ENABLED
    if (s_imguiEnabledAPI == this)
    {
        ImGui_ImplMetal_Shutdown();
        m_renderTarget->imGuiShutdown();
        ImGui::DestroyContext();
        s_imguiEnabledAPI = nullptr;
    }
    #endif

    if (m_shaderLibrary != nil)
        [m_shaderLibrary release];

    [m_renderPassDescriptor release];
    [m_commandQueue release];
    [m_mtlDevice release];
}}

MetalGraphicAPI::MetalGraphicAPI(const SharedPtr<Window>& renderTarget)
{
    if (renderTarget)
        setRenderTarget(renderTarget);
}

}