/*
 * ---------------------------------------------------
 * MetalCommandBuffer.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/08 10:37:29
 * ---------------------------------------------------
 */

#include "Graphics/Framebuffer.hpp"

#include "Metal/MetalCommandBuffer.hpp"
#include "Metal/MetalBuffer.hpp"
#include "Metal/MetalEnums.hpp"
#include "Metal/MetalTexture.hpp"
#include "MetalGraphicsPipeline.hpp"

#import <Metal/Metal.h>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <cassert>
    #include <memory>
#endif

namespace gfx
{

MetalCommandBuffer::MetalCommandBuffer(const id<MTLCommandQueue>& queue) { @autoreleasepool
{
    m_mtlCommandBuffer = [[queue commandBuffer] retain];
}}

void MetalCommandBuffer::beginRenderPass(const Framebuffer& framebuffer) { @autoreleasepool
{
    MTLRenderPassDescriptor* renderPassDescriptor = [[[MTLRenderPassDescriptor alloc] init] autorelease];

    for (int i = 0; auto& colorAttachment : framebuffer.colorAttachments)
    {
        const MetalTexture& mtlTexture = dynamic_cast<const MetalTexture&>(*colorAttachment.texture);
        renderPassDescriptor.colorAttachments[0].loadAction = toMTLLoadAction(colorAttachment.loadAction);
        renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
        renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(colorAttachment.clearColor[0],
                                                                                colorAttachment.clearColor[1],
                                                                                colorAttachment.clearColor[2],
                                                                                colorAttachment.clearColor[3]);
        renderPassDescriptor.colorAttachments[i].texture = mtlTexture.mtltexture();
    }
    if (auto& depthAttachment = framebuffer.depthAttachment)
    {
        const MetalTexture& mtlTexture = dynamic_cast<const MetalTexture&>(*depthAttachment->texture);
        renderPassDescriptor.depthAttachment.loadAction = toMTLLoadAction(depthAttachment->loadAction);
        renderPassDescriptor.depthAttachment.storeAction = MTLStoreActionStore;
        renderPassDescriptor.depthAttachment.clearDepth = depthAttachment->clearDepth;
        renderPassDescriptor.depthAttachment.texture = mtlTexture.mtltexture();
    }

    m_commandEncoder = [[m_mtlCommandBuffer renderCommandEncoderWithDescriptor: renderPassDescriptor] retain];
}}

void MetalCommandBuffer::usePipeline(const ext::shared_ptr<const GraphicsPipeline>& _graphicsPipeline) { @autoreleasepool
{
    auto graphicsPipeline = ext::dynamic_pointer_cast<const MetalGraphicsPipeline>(_graphicsPipeline);
    assert(graphicsPipeline);

    m_usedPipelines.insert(graphicsPipeline);

    [m_commandEncoder setRenderPipelineState:graphicsPipeline->renderPipelineState()];
    if (graphicsPipeline->depthStencilState() != nil)
        [m_commandEncoder setDepthStencilState:graphicsPipeline->depthStencilState()];
}}

void MetalCommandBuffer::useVertexBuffer(const ext::shared_ptr<const Buffer>& aBuffer) { @autoreleasepool
{
    auto buffer = ext::dynamic_pointer_cast<const MetalBuffer>(aBuffer);
    assert(buffer);

    m_usedBuffers.insert(buffer);

    [m_commandEncoder setVertexBuffer:buffer->mtlBuffer() offset:0 atIndex:0];
}}

void MetalCommandBuffer::drawVertices(uint32_t start, uint32_t count) { @autoreleasepool
{
    [m_commandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:start vertexCount:count];
}}

void MetalCommandBuffer::endRenderPass(void) { @autoreleasepool
{
    assert(m_commandEncoder);
    [m_commandEncoder endEncoding];
    [m_commandEncoder release];
    m_commandEncoder = nil;
}}

MetalCommandBuffer::~MetalCommandBuffer() { @autoreleasepool
{
    if (m_commandEncoder)
        [m_commandEncoder release];
    if (m_mtlCommandBuffer)
        [m_mtlCommandBuffer release];
}}

}
