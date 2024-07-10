/*
 * ---------------------------------------------------
 * MetalGraphicAPI.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/19 20:32:51
 * ---------------------------------------------------
 */

#include "GraphicAPI/Metal/MetalGraphicAPI.hpp"

#include "GraphicAPI/Metal/MetalBuffer.hpp"
#include "GraphicAPI/Metal/MetalFrameBuffer.hpp"
#include "GraphicAPI/Metal/MetalShader.hpp"
#include "GraphicAPI/Metal/MetalTexture.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Event.hpp"
#include "Graphics/FrameBuffer.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "GraphicAPI/Metal/MetalGraphicPipeline.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Texture.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Func.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/String.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/StructLayout.hpp"
#include "UtilsCPP/Types.hpp"
#include "UtilsCPP/UniquePtr.hpp"
#include "Window/MetalWindow.hpp"
#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>
#include <cassert>
#include <cstddef>
#include "Graphics/Error.hpp"

namespace gfx
{

MetalGraphicAPI::MetalGraphicAPI(const utils::SharedPtr<Window>& window)
    : m_window(window.forceDynamicCast<MetalWindow>()) { @autoreleasepool
{
    m_mtlDevice = MTLCreateSystemDefaultDevice();
    assert(m_mtlDevice); // TODO use throw

    m_window->setGraphicAPI(this);
    
    m_commandQueue = [m_mtlDevice newCommandQueue];
    assert(m_commandQueue); // TODO use throw
}}

utils::SharedPtr<Shader> MetalGraphicAPI::newShader(const Shader::MetalShaderDescriptor& shaderDescriptor, const Shader::OpenGLShaderDescriptor& openGLShaderDescriptor) const
{
    (void)openGLShaderDescriptor;
    return utils::makeShared<MetalShader>(m_mtlDevice, shaderDescriptor).staticCast<Shader>();
}

utils::SharedPtr<GraphicPipeline> MetalGraphicAPI::newGraphicsPipeline(const GraphicPipeline::Descriptor& descriptor) const
{
    return utils::makeShared<MetalGraphicPipeline>(m_mtlDevice, descriptor).staticCast<GraphicPipeline>();
}

utils::SharedPtr<Buffer> MetalGraphicAPI::newBuffer(const Buffer::Descriptor& descriptor) const
{
    return utils::makeShared<MetalBuffer>(m_mtlDevice, descriptor).staticCast<Buffer>();
}

utils::SharedPtr<Texture> MetalGraphicAPI::newTexture(const Texture::Descriptor& descriptor) const
{
    return utils::makeShared<MetalTexture>(m_mtlDevice, descriptor).staticCast<Texture>();
}

utils::SharedPtr<FrameBuffer> MetalGraphicAPI::newFrameBuffer(const utils::SharedPtr<Texture>& colorTexture) const
{
    return utils::makeShared<MetalFrameBuffer>(colorTexture).staticCast<FrameBuffer>();
}

void MetalGraphicAPI::beginFrame() { @autoreleasepool
{
    m_commandBuffer = [[m_commandQueue commandBuffer] retain];
    assert(m_commandBuffer); // TODO use throw
}}

void MetalGraphicAPI::beginRenderPass() { @autoreleasepool
{
    m_window->makeCurrentDrawables();

    MTLRenderPassDescriptor* renderPassDescriptor = [[[MTLRenderPassDescriptor alloc] init] autorelease];
    
    renderPassDescriptor.colorAttachments[0].loadAction = m_nextPassLoadAction == LoadAction::clear ? MTLLoadActionClear : MTLLoadActionLoad;
    renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(m_nextPassClearColor.r, m_nextPassClearColor.g, m_nextPassClearColor.b, m_nextPassClearColor.a);
    renderPassDescriptor.colorAttachments[0].texture = m_window->currentDrawable().texture;

    renderPassDescriptor.depthAttachment.loadAction = m_nextPassLoadAction == LoadAction::clear ? MTLLoadActionClear : MTLLoadActionLoad;
    renderPassDescriptor.depthAttachment.clearDepth = 1.0;
    renderPassDescriptor.depthAttachment.storeAction = MTLStoreActionStore;
    renderPassDescriptor.depthAttachment.texture = m_window->currentDepthTexture().mtlTexture();

    m_commandEncoder = [[m_commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor] retain];
    if (!m_commandEncoder)
        throw RenderCommandEncoderCreationError();
}}

void MetalGraphicAPI::beginRenderPass(const utils::SharedPtr<FrameBuffer>& target) { @autoreleasepool
{
    m_frameBuffer = target.forceDynamicCast<MetalFrameBuffer>();
    MTLRenderPassDescriptor* renderPassDescriptor = [[[MTLRenderPassDescriptor alloc] init] autorelease];

    renderPassDescriptor.colorAttachments[0].loadAction = m_nextPassLoadAction == LoadAction::clear ? MTLLoadActionClear : MTLLoadActionLoad;
    renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(m_nextPassClearColor.r, m_nextPassClearColor.g, m_nextPassClearColor.b, m_nextPassClearColor.a);
    renderPassDescriptor.colorAttachments[0].texture = m_frameBuffer->mtlColorTexture()->mtlTexture();

    // TODO depth for frame buffer

    m_commandEncoder = [[m_commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor] retain];
    if (!m_commandEncoder)
        throw RenderCommandEncoderCreationError();
}}

void MetalGraphicAPI::useGraphicsPipeline(const utils::SharedPtr<GraphicPipeline>& pipeline) { @autoreleasepool
{
    m_graphicPipeline = pipeline.forceDynamicCast<MetalGraphicPipeline>();
    [m_commandEncoder setRenderPipelineState:m_graphicPipeline->renderPipelineState()];
    [m_commandEncoder setDepthStencilState:m_graphicPipeline->depthStencilState()];
}}

void MetalGraphicAPI::useVertexBuffer(const utils::SharedPtr<Buffer>& buffer) { @autoreleasepool
{
    m_vertexBuffer = buffer.forceDynamicCast<MetalBuffer>();
    [m_commandEncoder setVertexBuffer:m_vertexBuffer->mtlBuffer() offset:0 atIndex:0];
}}

void MetalGraphicAPI::drawVertices(utils::uint32 start, utils::uint32 count) { @autoreleasepool
{
    [m_commandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:start vertexCount:count];
}}

void MetalGraphicAPI::drawIndexedVertices(const utils::SharedPtr<Buffer>& buffer) { @autoreleasepool
{
    m_indexBuffer = buffer.forceDynamicCast<MetalBuffer>();
    [m_commandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                 indexCount:m_indexBuffer->size() / sizeof(utils::uint32)
                                  indexType:MTLIndexTypeUInt32
                                indexBuffer:m_indexBuffer->mtlBuffer()
                          indexBufferOffset:0];
}}

void MetalGraphicAPI::endRenderPass()
{
    [m_commandEncoder endEncoding];
    [m_commandEncoder release];

    if (m_frameBuffer == false)
    {
        [m_commandBuffer presentDrawable:m_window->currentDrawable()];
        m_window->clearCurrentDrawables();
    }
    m_indexBuffer.clear();
    m_vertexBuffer.clear();
    m_graphicPipeline.clear();
    m_frameBuffer.clear();
}

void MetalGraphicAPI::endFrame()
{
    [m_commandBuffer commit];
    [m_commandBuffer release];
}

MetalGraphicAPI::~MetalGraphicAPI() { @autoreleasepool
{
    [m_commandQueue release];
    [m_mtlDevice release];
}}

}
