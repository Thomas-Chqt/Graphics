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
#include "Graphics/Enums.hpp"
#include "Graphics/FrameBuffer.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "GraphicAPI/Metal/MetalGraphicPipeline.hpp"
#include "Graphics/IndexBuffer.hpp"
#include "Graphics/Platform.hpp"
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
using utils::uint32;
using utils::Array;

namespace gfx
{

MetalGraphicAPI::MetalGraphicAPI(const utils::SharedPtr<Window>& window)
{
    if (SharedPtr<MetalWindow> mtlWindow = window.dynamicCast<MetalWindow>())
        m_window = mtlWindow;
    else
        throw utils::RuntimeError("Window is not MetalWindow");

    m_mtlDevice = MTLCreateSystemDefaultDevice();
    assert(m_mtlDevice); // TODO use throw
    
    m_window->metalLayer().device = m_mtlDevice;

    m_commandQueue = [m_mtlDevice newCommandQueue];
    assert(m_commandQueue); // TODO use throw
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

#ifdef GFX_METAL_ENABLED
void MetalGraphicAPI::initMetalShaderLib(const utils::String& path)
{
    NSError *error;
    NSString* mtlShaderLibPath = [[[NSString alloc] initWithCString:path encoding:NSUTF8StringEncoding] autorelease];
    m_shaderLib = [m_mtlDevice newLibraryWithURL:[NSURL URLWithString: mtlShaderLibPath] error:&error];
}
#endif

utils::SharedPtr<VertexBuffer> MetalGraphicAPI::newVertexBuffer(void* data, utils::uint64 count, utils::uint32 vertexSize, const utils::Array<VertexBuffer::LayoutElement>& layout) const
{
    (void)layout;
    return SharedPtr<VertexBuffer>(new MetalVertexBuffer(m_mtlDevice, data, count * vertexSize));
}

SharedPtr<GraphicPipeline> MetalGraphicAPI::newGraphicsPipeline(const GraphicPipeline::Descriptor& descriptor) const { @autoreleasepool
{
    if (m_shaderLib == nullptr)
        throw MetalShaderLibNotInitError();
    return SharedPtr<GraphicPipeline>(new MetalGraphicPipeline(m_mtlDevice, m_shaderLib, descriptor));
}}

SharedPtr<IndexBuffer> MetalGraphicAPI::newIndexBuffer(const Array<uint32>& indices) const
{
    return SharedPtr<IndexBuffer>(new MetalIndexBuffer(m_mtlDevice, indices));
}

SharedPtr<Texture> MetalGraphicAPI::newTexture(const Texture::Descriptor& descriptor) const { @autoreleasepool
{
    MTLTextureDescriptor* textureDescriptor = [[[MTLTextureDescriptor alloc] init] autorelease];
    textureDescriptor.width = descriptor.width;
    textureDescriptor.height = descriptor.height;
    textureDescriptor.pixelFormat = (MTLPixelFormat)toMetalPixelFormat(descriptor.pixelFormat);
    return SharedPtr<Texture>(new MetalTexture(m_mtlDevice, textureDescriptor));
}}

utils::SharedPtr<FrameBuffer> MetalGraphicAPI::newFrameBuffer(const utils::SharedPtr<Texture>& colorTexture) const
{
    return SharedPtr<FrameBuffer>(new MetalFrameBuffer(colorTexture));
}

void MetalGraphicAPI::beginFrame()
{
    m_commandBuffer = [[m_commandQueue commandBuffer] retain];
    assert(m_commandBuffer); // TODO use throw
}

void MetalGraphicAPI::beginOnScreenRenderPass() { @autoreleasepool
{
    m_currentDrawable = [[m_window->metalLayer() nextDrawable] retain];

    MTLRenderPassDescriptor* renderPassDescriptor = [[[MTLRenderPassDescriptor alloc] init] autorelease];
    
    renderPassDescriptor.colorAttachments[0].loadAction = m_nextPassLoadAction == LoadAction::clear ? MTLLoadActionClear : MTLLoadActionLoad;
    renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(m_nextPassClearColor.r, m_nextPassClearColor.g, m_nextPassClearColor.b, m_nextPassClearColor.a);
    renderPassDescriptor.colorAttachments[0].texture = m_currentDrawable.texture;

    #ifdef GFX_IMGUI_ENABLED
    if (s_imguiEnabledAPI == this)
    {
        m_window->imGuiNewFrame();
        ImGui_ImplMetal_NewFrame(renderPassDescriptor);
        ImGui::NewFrame();
    }
    #endif

    m_commandEncoder = [[m_commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor] retain];
    if (!m_commandEncoder)
        throw RenderCommandEncoderCreationError();
}}

void MetalGraphicAPI::beginOffScreenRenderPass(const utils::SharedPtr<FrameBuffer>& frameBuffer)
{
    if (utils::SharedPtr<MetalFrameBuffer> mtlFrameBuffer = frameBuffer.dynamicCast<MetalFrameBuffer>())
    {
        MTLRenderPassDescriptor* renderPassDescriptor = [[[MTLRenderPassDescriptor alloc] init] autorelease];
    
        renderPassDescriptor.colorAttachments[0].loadAction = m_nextPassLoadAction == LoadAction::clear ? MTLLoadActionClear : MTLLoadActionLoad;
        renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
        renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(m_nextPassClearColor.r, m_nextPassClearColor.g, m_nextPassClearColor.b, m_nextPassClearColor.a);
        renderPassDescriptor.colorAttachments[0].texture = mtlFrameBuffer->mtlColorTexture()->mtlTexture();

        m_commandEncoder = [[m_commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor] retain];
        if (!m_commandEncoder)
            throw RenderCommandEncoderCreationError();

        m_renderPassObjects.append(UniquePtr<utils::SharedPtrBase>(new SharedPtr<FrameBuffer>(frameBuffer)));
    }
    else
        throw utils::RuntimeError("FrameBuffer is not MetalFrameBuffer");
}

void MetalGraphicAPI::useGraphicsPipeline(const utils::SharedPtr<GraphicPipeline>& graphicsPipeline) { @autoreleasepool
{
    if (utils::SharedPtr<MetalGraphicPipeline> mtlGraphicsPipeline = graphicsPipeline.dynamicCast<MetalGraphicPipeline>())
    {
        [m_commandEncoder setRenderPipelineState:mtlGraphicsPipeline->renderPipelineState()];
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

void MetalGraphicAPI::endOnScreenRenderPass()
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

    [m_commandEncoder endEncoding];
    [m_commandEncoder release];

    [m_commandBuffer presentDrawable:m_currentDrawable];
    [m_currentDrawable release];

    m_renderPassObjects.clear();
}

void MetalGraphicAPI::endOffScreeRenderPass()
{
    [m_commandEncoder endEncoding];
    [m_commandEncoder release];
    m_renderPassObjects.clear();
}

void MetalGraphicAPI::endFrame() { @autoreleasepool
{
    [m_commandBuffer commit];
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

    if (m_shaderLib)
        [m_shaderLib release];
    [m_commandQueue release];
    [m_mtlDevice release];
}}

}
