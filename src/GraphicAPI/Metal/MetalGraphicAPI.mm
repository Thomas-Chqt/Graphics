/*
 * ---------------------------------------------------
 * MetalGraphicAPI.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/19 20:32:51
 * ---------------------------------------------------
 */

#define OBJCPP

#include "Graphics/GraphicAPI.hpp"
#include "GraphicAPI/Metal/MetalGraphicAPI.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Logger/Logger.hpp"
#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>

using utils::SharedPtr;

namespace gfx
{

SharedPtr<GraphicAPI> GraphicAPI::newMetalGraphicAPI(const SharedPtr<Window>& renderTarget)
{
    return SharedPtr<GraphicAPI>(new MetalGraphicAPI(renderTarget));
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
    
    // NSError *error;
    // m_shaderLibrary = [m_mtlDevice newLibraryWithURL:[NSURL URLWithString: @"MTL_SHADER_LIB"] error:&error];
    // assert(m_shaderLibrary);

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

    // [m_shaderLibrary release];
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