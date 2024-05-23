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
#include "Graphics/GraphicPipeline.hpp"
#include "GraphicAPI/Metal/MetalGraphicPipeline.hpp"
#include "Graphics/IndexBuffer.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/ShaderLibrary.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/String.hpp"
#include <Foundation/Foundation.h>
#include "GraphicAPI/Metal/MetalVertexBuffer.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Logger/Logger.hpp"
#include "UtilsCPP/Types.hpp"
#include "Window/MetalWindow.hpp"
#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>
#include <cassert>
#include <cstddef>

using utils::SharedPtr;
using utils::String;
using utils::uint32;
using utils::Array;

#ifdef IMGUI_ENABLED
    class ImDrawData;
    bool ImGui_ImplMetal_Init(id<MTLDevice> device);
    void ImGui_ImplMetal_Shutdown();
    void ImGui_ImplMetal_NewFrame(MTLRenderPassDescriptor*);
    void ImGui_ImplMetal_RenderDrawData(ImDrawData*, id<MTLCommandBuffer>, id<MTLRenderCommandEncoder>);
    namespace ImGui { ImDrawData* GetDrawData(); }
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
        logFatal << "Window is not MetalWindow" << std::endl;

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
        m_renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
        m_renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
    }
    
    logDebug << "MetalGraphicAPI render target set to window " << renderTarget << std::endl;
}}

#ifdef IMGUI_ENABLED
void MetalGraphicAPI::useForImGui()
{
    assert(s_imguiEnabledAPI == nullptr && "Im gui is already using a graphic api object");
    assert(m_renderTarget && "Render target need to be set before initializing imgui");
    m_renderTarget->imGuiInit();
    ImGui_ImplMetal_Init(m_mtlDevice);
    s_imguiEnabledAPI = this;
}
#endif

void MetalGraphicAPI::setClearColor(float r, float g, float b, float a)
{
    m_renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(r, g, b, a);
}

utils::SharedPtr<VertexBuffer> MetalGraphicAPI::newVertexBuffer(void* data, utils::uint64 size, const VertexBuffer::LayoutBase& layout)
{
    (void)layout;
    return SharedPtr<VertexBuffer>(new MetalVertexBuffer(m_mtlDevice, data, size));
}

SharedPtr<GraphicPipeline> MetalGraphicAPI::newGraphicsPipeline(const String& vertexShaderName, const String& fragmentShaderName)
{
    if (m_shaderLibrary == nil)
    {
        NSError *error;
        NSString* mtlShaderLibPath = [[[NSString alloc] initWithCString:ShaderLibrary::shared().getMetalShaderLibPath() encoding:NSUTF8StringEncoding] autorelease];
        m_shaderLibrary = [m_mtlDevice newLibraryWithURL:[NSURL URLWithString: mtlShaderLibPath] error:&error];
        assert(m_shaderLibrary);
    }

    return SharedPtr<GraphicPipeline>(new MetalGraphicPipeline(m_mtlDevice, m_shaderLibrary, m_renderTarget->metalLayer(), vertexShaderName, fragmentShaderName));
}

SharedPtr<IndexBuffer> MetalGraphicAPI::newIndexBuffer(const Array<uint32>& indices)
{
    return SharedPtr<IndexBuffer>(new MetalIndexBuffer(m_mtlDevice, indices));
}

void MetalGraphicAPI::beginFrame()
{
    assert(m_frameAutoreleasePool == nullptr);

    m_frameAutoreleasePool = [[NSAutoreleasePool alloc] init];

    m_commandBuffer = [m_commandQueue commandBuffer];
    assert(m_commandBuffer);
	
	m_currentDrawable = [m_renderTarget->metalLayer() nextDrawable];
    assert(m_currentDrawable);

	m_renderPassDescriptor.colorAttachments[0].texture = m_currentDrawable.texture;

    m_commandEncoder = [m_commandBuffer renderCommandEncoderWithDescriptor:m_renderPassDescriptor];
    assert(m_commandEncoder);

#ifdef IMGUI_ENABLED
    if (s_imguiEnabledAPI == this)
    {
        ImGui_ImplMetal_NewFrame(m_renderPassDescriptor);
        m_renderTarget->imGuiNewFrame();
    }
#endif
}

void MetalGraphicAPI::useGraphicsPipeline(utils::SharedPtr<GraphicPipeline> graphicsPipeline)
{
    if (utils::SharedPtr<MetalGraphicPipeline> mtlGraphicsPipeline = graphicsPipeline.dynamicCast<MetalGraphicPipeline>())
        [m_commandEncoder setRenderPipelineState:mtlGraphicsPipeline->renderPipelineState()];
    else
        logFatal << "GraphicPipeline is not MetalGraphicPipeline" << std::endl;

}

void MetalGraphicAPI::useVertexBuffer(utils::SharedPtr<VertexBuffer> vertexBuffer)
{
    if (utils::SharedPtr<MetalVertexBuffer> mtlVertexBuffer = vertexBuffer.dynamicCast<MetalVertexBuffer>())
        [m_commandEncoder setVertexBuffer:mtlVertexBuffer->mtlBuffer() offset:0 atIndex:0];
    else
        logFatal << "VertexBuffer is not MetalVertexBuffer" << std::endl;

}

void MetalGraphicAPI::drawVertices(utils::uint32 start, utils::uint32 count)
{
    [m_commandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:start vertexCount:count];
}

void MetalGraphicAPI::drawIndexedVertices(utils::SharedPtr<IndexBuffer> indexBuffer)
{
    assert(m_frameAutoreleasePool != nil);

    if (utils::SharedPtr<MetalIndexBuffer> mtlIndexBuffer = indexBuffer.dynamicCast<MetalIndexBuffer>())
        [m_commandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                     indexCount:mtlIndexBuffer->indexCount()
                                      indexType:MTLIndexTypeUInt32
                                    indexBuffer:mtlIndexBuffer->mtlBuffer()
                              indexBufferOffset:0];
    else
        logFatal << "IndexBuffer is not MetalIndexBuffer" << std::endl;
}

void MetalGraphicAPI::endFrame()
{
    assert(m_frameAutoreleasePool != nullptr);

#ifdef IMGUI_ENABLED
    if (s_imguiEnabledAPI == this)
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), m_commandBuffer, m_commandEncoder);
#endif
    
    [m_commandEncoder endEncoding];

    [m_commandBuffer presentDrawable:m_currentDrawable];
    [m_commandBuffer commit];

    [m_frameAutoreleasePool drain];
    m_frameAutoreleasePool = nullptr;
}

MetalGraphicAPI::~MetalGraphicAPI() { @autoreleasepool
{
    if (m_frameAutoreleasePool != nullptr)
        [m_frameAutoreleasePool drain];

#ifdef IMGUI_ENABLED
    if (s_imguiEnabledAPI == this)
    {
        ImGui_ImplMetal_Shutdown();
        m_renderTarget->imGuiShutdown();
    }
#endif

    if (m_shaderLibrary != nil)
        [m_shaderLibrary release];
    [m_renderPassDescriptor release];
    [m_commandQueue release];
    [m_mtlDevice release];

    logDebug << "MetalGraphicAPI destructed" << std::endl;
}}

MetalGraphicAPI::MetalGraphicAPI(const SharedPtr<Window>& renderTarget)
{
    if (renderTarget)
        setRenderTarget(renderTarget);
    logDebug << "MetalGraphicAPI created" << std::endl;
}

}