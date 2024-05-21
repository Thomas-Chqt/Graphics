/*
 * ---------------------------------------------------
 * MetalGraphicAPI.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/19 20:32:51
 * ---------------------------------------------------
 */

#define OBJCPP

#include "Graphics/GraphicPipeline.hpp"
#include "GraphicAPI/Metal/MetalGraphicPipeline.hpp"
#include "Graphics/ShaderLibrary.hpp"
#include "UtilsCPP/String.hpp"
#include <Foundation/Foundation.h>
#include "GraphicAPI/Metal/MetalVertexBuffer.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "GraphicAPI/Metal/MetalGraphicAPI.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Logger/Logger.hpp"
#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>

using utils::SharedPtr;
using utils::String;

namespace gfx
{

SharedPtr<GraphicAPI> GraphicAPI::newMetalGraphicAPI(const SharedPtr<Window>& renderTarget)
{
    return SharedPtr<GraphicAPI>(new MetalGraphicAPI(renderTarget));
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

    return SharedPtr<GraphicPipeline>(new MetalGraphicPipeline(m_mtlDevice, m_shaderLibrary, m_renderTarget->getMetalLayer(), vertexShaderName, fragmentShaderName));
}

void MetalGraphicAPI::setRenderTarget(const utils::SharedPtr<Window>& renderTarget) { @autoreleasepool
{
    m_renderTarget = renderTarget;
    m_renderTarget->useMetal();

    if (m_mtlDevice == nullptr)
    {
        m_mtlDevice = MTLCreateSystemDefaultDevice();
        assert(m_mtlDevice);
    }
    
    m_renderTarget->getMetalLayer().device = m_mtlDevice;

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

void MetalGraphicAPI::setClearColor(float r, float g, float b, float a)
{
    m_renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(r, g, b, a);
}

void MetalGraphicAPI::beginFrame()
{
    assert(m_frameAutoreleasePool == nullptr);

    m_frameAutoreleasePool = [[NSAutoreleasePool alloc] init];

    m_commandBuffer = [m_commandQueue commandBuffer];
    assert(m_commandBuffer);
	
	m_currentDrawable = [m_renderTarget->getMetalLayer() nextDrawable];
    assert(m_currentDrawable);

	m_renderPassDescriptor.colorAttachments[0].texture = m_currentDrawable.texture;

    m_commandEncoder = [m_commandBuffer renderCommandEncoderWithDescriptor:m_renderPassDescriptor];
    assert(m_commandEncoder);

    [m_commandEncoder setViewport:(MTLViewport){0.0, 0.0, m_renderTarget->getMetalLayer().drawableSize.width, m_renderTarget->getMetalLayer().drawableSize.height, 0.0, 1.0 }];
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

void MetalGraphicAPI::endFrame()
{
    assert(m_frameAutoreleasePool != nullptr);
    
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