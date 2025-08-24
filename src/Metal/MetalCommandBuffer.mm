/*
 * ---------------------------------------------------
 * MetalCommandBuffer.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/08 10:37:29
 * ---------------------------------------------------
 */

#include "Graphics/Enums.hpp"
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

#import <Metal/Metal.h>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <algorithm>
    #include <cassert>
    #include <memory>
    #include <cstdint>
    #include <ranges>
    #include <utility>
#endif

#if defined(GFX_IMGUI_ENABLED)
    struct ImDrawData;
#endif

namespace gfx
{

MetalCommandBuffer::MetalCommandBuffer(MetalCommandBuffer&& other) noexcept
    : m_mtlCommandBuffer(ext::exchange(other.m_mtlCommandBuffer, nil)),
      m_commandEncoder(ext::exchange(other.m_commandEncoder, nil)),
      m_usedPipelines(ext::move(other.m_usedPipelines)),
      m_usedTextures(ext::move(other.m_usedTextures)),
      m_usedBuffers(ext::move(other.m_usedBuffers))
{
}

MetalCommandBuffer::MetalCommandBuffer(const id<MTLCommandQueue>& queue) { @autoreleasepool
{
    m_mtlCommandBuffer = [[queue commandBuffer] retain];
}}

void MetalCommandBuffer::beginRenderPass(const Framebuffer& framebuffer) { @autoreleasepool
{
    assert(m_commandEncoder == nil);

    MTLRenderPassDescriptor* renderPassDescriptor = [[[MTLRenderPassDescriptor alloc] init] autorelease];

    for (int i = 0; auto& colorAttachment : framebuffer.colorAttachments)
    {
        auto texture = ext::dynamic_pointer_cast<const MetalTexture>(colorAttachment.texture);
        assert(texture);
        renderPassDescriptor.colorAttachments[0].loadAction = toMTLLoadAction(colorAttachment.loadAction);
        renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
        renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(
            colorAttachment.clearColor[0], colorAttachment.clearColor[1],
            colorAttachment.clearColor[2], colorAttachment.clearColor[3]);
        renderPassDescriptor.colorAttachments[i].texture = texture->mtltexture();
        m_usedTextures.insert(texture);
    }

    if (auto& depthAttachment = framebuffer.depthAttachment)
    {
        auto texture = ext::dynamic_pointer_cast<const MetalTexture>(depthAttachment->texture);
        renderPassDescriptor.depthAttachment.loadAction = toMTLLoadAction(depthAttachment->loadAction);
        renderPassDescriptor.depthAttachment.storeAction = MTLStoreActionStore;
        renderPassDescriptor.depthAttachment.clearDepth = depthAttachment->clearDepth;
        renderPassDescriptor.depthAttachment.texture = texture->mtltexture();
        m_usedTextures.insert(texture);
    }

    m_commandEncoder = [[m_mtlCommandBuffer renderCommandEncoderWithDescriptor: renderPassDescriptor] retain];
}}

void MetalCommandBuffer::usePipeline(const ext::shared_ptr<const GraphicsPipeline>& _graphicsPipeline) { @autoreleasepool
{
    auto graphicsPipeline = ext::dynamic_pointer_cast<const MetalGraphicsPipeline>(_graphicsPipeline);
    assert(graphicsPipeline);

    m_usedPipelines.insert(graphicsPipeline);

    assert([m_commandEncoder conformsToProtocol:@protocol(MTLRenderCommandEncoder)]);
    [(id<MTLRenderCommandEncoder>)m_commandEncoder setRenderPipelineState:graphicsPipeline->renderPipelineState()];
    if (graphicsPipeline->depthStencilState() != nil)
        [(id<MTLRenderCommandEncoder>)m_commandEncoder setDepthStencilState:graphicsPipeline->depthStencilState()];
}}

void MetalCommandBuffer::useVertexBuffer(const ext::shared_ptr<Buffer>& aBuffer) { @autoreleasepool
{
    auto buffer = ext::dynamic_pointer_cast<MetalBuffer>(aBuffer);
    assert(buffer);


    m_usedBuffers.insert(buffer);

    assert([m_commandEncoder conformsToProtocol:@protocol(MTLRenderCommandEncoder)]);
    [(id<MTLRenderCommandEncoder>)m_commandEncoder setVertexBuffer:buffer->mtlBuffer() offset:0 atIndex:5];
}}

void MetalCommandBuffer::setParameterBlock(const ParameterBlock& aPBlock, uint32_t index) { @autoreleasepool
{
    const auto& pBlock = dynamic_cast<const MetalParameterBlock&>(aPBlock);

    assert([m_commandEncoder conformsToProtocol:@protocol(MTLRenderCommandEncoder)]);

    for (const auto& [binding, buffer] : pBlock.encodedBuffers())
        [(id<MTLRenderCommandEncoder>)m_commandEncoder useResource:buffer->mtlBuffer()
                                                             usage:toMTLResourceUsage(binding.usages)
                                                            stages:toMTLRenderStages(binding.usages)];

    if (ext::ranges::any_of(pBlock.encodedBuffers(),  [](auto& pair) { return (bool)(pair.first.usages & BindingUsage::vertexRead) || (bool)(pair.first.usages & BindingUsage::vertexWrite); }))
    {
        [(id<MTLRenderCommandEncoder>)m_commandEncoder setVertexBuffer:pBlock.argumentBuffer().mtlBuffer()
                                                                offset:pBlock.offset()
                                                               atIndex:index];
    }

    if (ext::ranges::any_of(pBlock.encodedBuffers(),  [](auto& pair) { return (bool)(pair.first.usages & BindingUsage::fragmentRead) || (bool)(pair.first.usages & BindingUsage::fragmentWrite); }))
    {
        [(id<MTLRenderCommandEncoder>)m_commandEncoder setFragmentBuffer:pBlock.argumentBuffer().mtlBuffer()
                                                                  offset:pBlock.offset()
                                                                 atIndex:index];
    }

    m_usedBuffers.insert_range(pBlock.encodedBuffers()   | ext::views::transform([](auto& pair){ return pair.second; }));
}}

void MetalCommandBuffer::drawVertices(uint32_t start, uint32_t count) { @autoreleasepool
{
    assert([m_commandEncoder conformsToProtocol:@protocol(MTLRenderCommandEncoder)]);
    [(id<MTLRenderCommandEncoder>)m_commandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:start vertexCount:count];
}}

void MetalCommandBuffer::drawIndexedVertices(const ext::shared_ptr<Buffer>& buffer) { @autoreleasepool
{
    auto idxBuffer = ext::dynamic_pointer_cast<MetalBuffer>(buffer);
    assert(idxBuffer);

    assert([m_commandEncoder conformsToProtocol:@protocol(MTLRenderCommandEncoder)]);

    m_usedBuffers.insert(idxBuffer);

    [(id<MTLRenderCommandEncoder>)m_commandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                                              indexCount:idxBuffer->size() / sizeof(uint32_t)
                                                               indexType:MTLIndexTypeUInt32
                                                             indexBuffer:idxBuffer->mtlBuffer()
                                                       indexBufferOffset:0];
}}

#if defined(GFX_IMGUI_ENABLED)
void MetalCommandBuffer::imGuiRenderDrawData(ImDrawData* drawData) const { @autoreleasepool
{
    assert([m_commandEncoder conformsToProtocol:@protocol(MTLRenderCommandEncoder)]);
    ImGui_ImplMetal_RenderDrawData(drawData, m_mtlCommandBuffer, (id<MTLRenderCommandEncoder>)m_commandEncoder);
}}
#endif

void MetalCommandBuffer::endRenderPass() { @autoreleasepool
{
    assert(m_commandEncoder);
    [m_commandEncoder endEncoding];
    [m_commandEncoder release];
    m_commandEncoder = nil;
}}

MetalCommandBuffer::~MetalCommandBuffer() { @autoreleasepool
{
    m_usedBuffers.clear();
    m_usedTextures.clear();
    m_usedPipelines.clear();
    if (m_commandEncoder != nil)
        [m_mtlCommandBuffer release];
    if (m_mtlCommandBuffer != nil)
        [m_mtlCommandBuffer release];
}}

MetalCommandBuffer& MetalCommandBuffer::operator = (MetalCommandBuffer&& other) noexcept
{
    if (this != &other)
    {
        if (m_commandEncoder != nil)
            [m_mtlCommandBuffer release];
        if (m_mtlCommandBuffer != nil)
            [m_mtlCommandBuffer release];

        m_mtlCommandBuffer = ext::exchange(other.m_mtlCommandBuffer, nil);
        m_commandEncoder = ext::exchange(other.m_commandEncoder, nil);
        m_usedPipelines = ext::move(other.m_usedPipelines);
        m_usedTextures = ext::move(other.m_usedTextures);
        m_usedBuffers = ext::move(other.m_usedBuffers);
    }
    return *this;
}

}
