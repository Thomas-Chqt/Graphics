/*
 * ---------------------------------------------------
 * MetalCommandBuffer.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/08 10:37:29
 * ---------------------------------------------------
 */

#include "Graphics/Framebuffer.hpp"
#include "Graphics/GraphicsPipeline.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/ParameterBlock.hpp"

#include "Metal/MetalCommandBuffer.hpp"
#include "Metal/MetalBuffer.hpp"
#include "Metal/MetalTexture.hpp"
#include "Metal/imgui_impl_metal.h"
#include "Metal/MetalGraphicsPipeline.hpp"
#include "Metal/MetalParameterBlock.hpp"

#import "Metal/MetalEnums.h"
#include <algorithm>

#import <Metal/Metal.h>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <cassert>
    #include <memory>
    #include <cstdint>
    #include <ranges>
#endif

#if defined(GFX_IMGUI_ENABLED)
    struct ImDrawData;
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

    [m_commandEncoder setVertexBuffer:buffer->mtlBuffer() offset:0 atIndex:5];
}}

void MetalCommandBuffer::setParameterBlock(const ParameterBlock& aPBlock, uint32_t index) { @autoreleasepool
{
    const auto& pBlock = dynamic_cast<const MetalParameterBlock&>(aPBlock);

    for (const auto& [binding, buffer] : pBlock.encodedBuffers())
        [m_commandEncoder useResource:buffer->mtlBuffer()
                                usage:toMTLResourceUsage(binding.usages)
                               stages:toMTLRenderStages(binding.usages)];
    if (ext::ranges::any_of(pBlock.encodedBuffers(), [](auto& pair){ return (bool)(pair.first.usages & BindingUsage::vertexRead) || (bool)(pair.first.usages & BindingUsage::vertexWrite); }))
        [m_commandEncoder setVertexBuffer:pBlock.argumentBuffer().mtlBuffer() offset:pBlock.offset() atIndex:index];
    if (ext::ranges::any_of(pBlock.encodedBuffers(), [](auto& pair){ return (bool)(pair.first.usages & BindingUsage::fragmentRead) || (bool)(pair.first.usages & BindingUsage::fragmentWrite); }))
        [m_commandEncoder setFragmentBuffer:pBlock.argumentBuffer().mtlBuffer() offset:pBlock.offset() atIndex:index];

    m_usedBuffers.insert_range(pBlock.encodedBuffers() | ext::views::transform([](auto& pair){ return pair.second; }));
}}

void MetalCommandBuffer::drawVertices(uint32_t start, uint32_t count) { @autoreleasepool
{
    [m_commandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:start vertexCount:count];
}}

void MetalCommandBuffer::drawIndexedVertices(const ext::shared_ptr<const Buffer>& buffer) { @autoreleasepool
{
    auto idxBuffer = ext::dynamic_pointer_cast<const MetalBuffer>(buffer);
    assert(idxBuffer);

    m_usedBuffers.insert(idxBuffer);

    [m_commandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                 indexCount:idxBuffer->size() / sizeof(uint32_t)
                                  indexType:MTLIndexTypeUInt32
                                indexBuffer:idxBuffer->mtlBuffer()
                          indexBufferOffset:0];
}}

#if defined(GFX_IMGUI_ENABLED)
void MetalCommandBuffer::imGuiRenderDrawData(ImDrawData* drawData) const
{
    ImGui_ImplMetal_RenderDrawData(drawData, m_mtlCommandBuffer, m_commandEncoder);
}
#endif

void MetalCommandBuffer::endRenderPass(void) { @autoreleasepool
{
    assert(m_commandEncoder);
    [m_commandEncoder endEncoding];
    [m_commandEncoder release];
    m_commandEncoder = nil;
}}

MetalCommandBuffer::~MetalCommandBuffer() { @autoreleasepool
{
    m_usedBuffers.clear();
    m_usedPipelines.clear();
    assert(m_commandEncoder == nullptr);
    [m_mtlCommandBuffer release];
}}

}
