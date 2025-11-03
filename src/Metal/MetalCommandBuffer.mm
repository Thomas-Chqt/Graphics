/*
 * ---------------------------------------------------
 * MetalCommandBuffer.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/08 10:37:29
 * ---------------------------------------------------
 */

#include "Graphics/CommandBuffer.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Framebuffer.hpp"
#include "Graphics/GraphicsPipeline.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/ParameterBlock.hpp"

#include "Metal/MetalCommandBuffer.hpp"
#include "Metal/MetalBuffer.hpp"
#include "Metal/MetalSampler.hpp"
#include "Metal/MetalTexture.hpp"
#include "Metal/imgui_impl_metal.hpp"
#include "Metal/MetalGraphicsPipeline.hpp"
#include "Metal/MetalParameterBlock.hpp"
#include "Metal/MetalDrawable.hpp"
#include "Metal/MetalCommandBufferPool.hpp"

#import "Metal/MetalEnums.h"

namespace gfx
{

MetalCommandBuffer::MetalCommandBuffer(MetalCommandBuffer&& other) noexcept
    : CommandBuffer(std::move(other)),
      m_sourcePool(std::exchange(other.m_sourcePool, nullptr)),
      m_mtlCommandBuffer(std::exchange(other.m_mtlCommandBuffer, nil)),
      m_commandEncoder(std::exchange(other.m_commandEncoder, nil)),
      m_usedPipelines(std::move(other.m_usedPipelines)),
      m_usedTextures(std::move(other.m_usedTextures)),
      m_usedBuffers(std::move(other.m_usedBuffers)),
      m_usedSamplers(std::move(other.m_usedSamplers)),
      m_usedPBlock(std::move(other.m_usedPBlock))
{
}

MetalCommandBuffer::MetalCommandBuffer(const id<MTLCommandQueue>& queue, MetalCommandBufferPool* commandPool)
    : m_sourcePool(commandPool) { @autoreleasepool
{
    m_mtlCommandBuffer = [[queue commandBuffer] retain];
}}

CommandBufferPool* MetalCommandBuffer::pool()
{
    return m_sourcePool;
}

void MetalCommandBuffer::beginRenderPass(const Framebuffer& framebuffer) { @autoreleasepool
{
    assert(m_commandEncoder == nil);

    MTLRenderPassDescriptor* renderPassDescriptor = [[[MTLRenderPassDescriptor alloc] init] autorelease];

    for (int i = 0; auto& colorAttachment : framebuffer.colorAttachments)
    {
        auto texture = std::dynamic_pointer_cast<MetalTexture>(colorAttachment.texture);
        assert(texture);
        renderPassDescriptor.colorAttachments[i].loadAction = toMTLLoadAction(colorAttachment.loadAction);
        renderPassDescriptor.colorAttachments[i].storeAction = MTLStoreActionStore;
        renderPassDescriptor.colorAttachments[i].clearColor = MTLClearColorMake(
            colorAttachment.clearColor[0], colorAttachment.clearColor[1],
            colorAttachment.clearColor[2], colorAttachment.clearColor[3]);
        renderPassDescriptor.colorAttachments[i].texture = texture->mtltexture();
        m_usedTextures.insert(texture);
    }

    if (auto& depthAttachment = framebuffer.depthAttachment)
    {
        auto texture = std::dynamic_pointer_cast<MetalTexture>(depthAttachment->texture);
        renderPassDescriptor.depthAttachment.loadAction = toMTLLoadAction(depthAttachment->loadAction);
        renderPassDescriptor.depthAttachment.storeAction = MTLStoreActionStore;
        renderPassDescriptor.depthAttachment.clearDepth = depthAttachment->clearDepth;
        renderPassDescriptor.depthAttachment.texture = texture->mtltexture();
        m_usedTextures.insert(texture);
    }

    m_commandEncoder = [[m_mtlCommandBuffer renderCommandEncoderWithDescriptor: renderPassDescriptor] retain];
}}

void MetalCommandBuffer::usePipeline(const std::shared_ptr<const GraphicsPipeline>& _graphicsPipeline) { @autoreleasepool
{
    auto graphicsPipeline = std::dynamic_pointer_cast<const MetalGraphicsPipeline>(_graphicsPipeline);
    assert(graphicsPipeline);

    assert([m_commandEncoder conformsToProtocol:@protocol(MTLRenderCommandEncoder)]);
    auto renderCommandEncoder = (id<MTLRenderCommandEncoder>)m_commandEncoder;

    [renderCommandEncoder setRenderPipelineState:graphicsPipeline->renderPipelineState()];
    [renderCommandEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
    [renderCommandEncoder setCullMode:toMTLCullMode(graphicsPipeline->cullMode())];
    if (graphicsPipeline->depthStencilState() != nil)
        [renderCommandEncoder setDepthStencilState:graphicsPipeline->depthStencilState()];

    m_usedPipelines.insert(graphicsPipeline);
}}

void MetalCommandBuffer::useVertexBuffer(const std::shared_ptr<Buffer>& aBuffer) { @autoreleasepool
{
    auto buffer = std::dynamic_pointer_cast<MetalBuffer>(aBuffer);
    assert(buffer);

    assert([m_commandEncoder conformsToProtocol:@protocol(MTLRenderCommandEncoder)]);
    auto renderCommandEncoder = (id<MTLRenderCommandEncoder>)m_commandEncoder;

    [renderCommandEncoder setVertexBuffer:buffer->mtlBuffer() offset:0 atIndex:5];

    m_usedBuffers.insert(buffer);
}}

void MetalCommandBuffer::setParameterBlock(const std::shared_ptr<const ParameterBlock>& aPBlock, uint32_t index) { @autoreleasepool
{
    const auto& pBlock = std::dynamic_pointer_cast<const MetalParameterBlock>(aPBlock);

    assert([m_commandEncoder conformsToProtocol:@protocol(MTLRenderCommandEncoder)]);
    auto renderCommandEncoder = (id<MTLRenderCommandEncoder>)m_commandEncoder;

    for (const auto& [buffer, binding] : pBlock->encodedBuffers())
        [renderCommandEncoder useResource:buffer->mtlBuffer() usage:toMTLResourceUsage(binding.usages) stages:toMTLRenderStages(binding.usages)];

    for (const auto& [texture, binding] : pBlock->encodedTextures())
        [renderCommandEncoder useResource:texture->mtltexture() usage:toMTLResourceUsage(binding.usages) stages:toMTLRenderStages(binding.usages)];

    if (std::ranges::any_of(pBlock->encodedBuffers(), [](auto& pair) { return pair.second.usages & BindingUsage::vertexRead || pair.second.usages & BindingUsage::vertexWrite; }) ||
        std::ranges::any_of(pBlock->encodedTextures(), [](auto& pair) { return pair.second.usages & BindingUsage::vertexRead || pair.second.usages & BindingUsage::vertexWrite; }) ||
        std::ranges::any_of(pBlock->encodedSamplers(), [](auto& pair) { return pair.second.usages & BindingUsage::vertexRead || pair.second.usages & BindingUsage::vertexWrite; }))
    {
        [renderCommandEncoder setVertexBuffer:pBlock->argumentBuffer().mtlBuffer() offset:pBlock->offset() atIndex:index];
    }

    if (std::ranges::any_of(pBlock->encodedBuffers(), [](auto& pair) { return pair.second.usages & BindingUsage::fragmentRead || pair.second.usages & BindingUsage::fragmentWrite; }) ||
        std::ranges::any_of(pBlock->encodedTextures(), [](auto& pair) { return pair.second.usages & BindingUsage::fragmentRead || pair.second.usages & BindingUsage::fragmentWrite; }) ||
        std::ranges::any_of(pBlock->encodedSamplers(), [](auto& pair) { return pair.second.usages & BindingUsage::fragmentRead || pair.second.usages & BindingUsage::fragmentWrite; }))
    {
        [renderCommandEncoder setFragmentBuffer:pBlock->argumentBuffer().mtlBuffer() offset:pBlock->offset() atIndex:index];
    }

    m_usedBuffers.insert_range(pBlock->encodedBuffers() | std::views::transform([](auto& pair) -> std::shared_ptr<MetalBuffer> { return pair.first; }));
    m_usedTextures.insert_range(pBlock->encodedTextures() | std::views::transform([](auto& pair) -> std::shared_ptr<MetalTexture> { return pair.first; }));
    m_usedSamplers.insert_range(pBlock->encodedSamplers() | std::views::transform([](auto& pair) -> std::shared_ptr<MetalSampler> { return pair.first; }));
    m_usedPBlock.insert(pBlock);
}}

void MetalCommandBuffer::drawVertices(uint32_t start, uint32_t count) { @autoreleasepool
{
    assert([m_commandEncoder conformsToProtocol:@protocol(MTLRenderCommandEncoder)]);
    auto renderCommandEncoder = (id<MTLRenderCommandEncoder>)m_commandEncoder;

    [renderCommandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:start vertexCount:count];
}}

void MetalCommandBuffer::drawIndexedVertices(const std::shared_ptr<Buffer>& buffer) { @autoreleasepool
{
    auto idxBuffer = std::dynamic_pointer_cast<MetalBuffer>(buffer);
    assert(idxBuffer);

    assert([m_commandEncoder conformsToProtocol:@protocol(MTLRenderCommandEncoder)]);
    auto renderCommandEncoder = (id<MTLRenderCommandEncoder>)m_commandEncoder;

    [renderCommandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                     indexCount:idxBuffer->size() / sizeof(uint32_t)
                                      indexType:MTLIndexTypeUInt32
                                    indexBuffer:idxBuffer->mtlBuffer()
                              indexBufferOffset:0];

    m_usedBuffers.insert(idxBuffer);
}}

#if defined(GFX_IMGUI_ENABLED)
void MetalCommandBuffer::imGuiRenderDrawData(ImDrawData* drawData) const { @autoreleasepool
{
    assert([m_commandEncoder conformsToProtocol:@protocol(MTLRenderCommandEncoder)]);
    auto renderCommandEncoder = (id<MTLRenderCommandEncoder>)m_commandEncoder;
    ImGui_ImplMetal_RenderDrawData(drawData, m_mtlCommandBuffer, renderCommandEncoder);
}}
#endif

void MetalCommandBuffer::endRenderPass() { @autoreleasepool
{
    assert(m_commandEncoder);
    [m_commandEncoder endEncoding];
    [m_commandEncoder release];
    m_commandEncoder = nil;
}}

void MetalCommandBuffer::beginBlitPass() { @autoreleasepool
{
    assert(m_commandEncoder == nil);
    m_commandEncoder = [[m_mtlCommandBuffer blitCommandEncoder] retain];
}}

void MetalCommandBuffer::copyBufferToBuffer(const std::shared_ptr<Buffer>& aSrc, const std::shared_ptr<Buffer>& aDst, size_t size) { @autoreleasepool
{
    auto src = std::dynamic_pointer_cast<MetalBuffer>(aSrc);
    assert(src);

    auto dst = std::dynamic_pointer_cast<MetalBuffer>(aDst);
    assert(dst);

    assert(src->usages() & BufferUsage::copySource && dst->usages() & BufferUsage::copyDestination);

    assert([m_commandEncoder conformsToProtocol:@protocol(MTLBlitCommandEncoder)]);
    auto blitCommandEncoder = (id<MTLBlitCommandEncoder>)m_commandEncoder;

    [blitCommandEncoder copyFromBuffer:src->mtlBuffer() sourceOffset:0 toBuffer:dst->mtlBuffer() destinationOffset:0 size:size];

    m_usedBuffers.insert(src);
    m_usedBuffers.insert(dst);
}}

void MetalCommandBuffer::copyBufferToTexture(const std::shared_ptr<Buffer>& aBuffer, size_t bufferOffset, const std::shared_ptr<Texture>& aTexture, uint32_t layerIndex) { @autoreleasepool
{
    auto buffer = std::dynamic_pointer_cast<MetalBuffer>(aBuffer);
    assert(buffer);

    auto texture = std::dynamic_pointer_cast<MetalTexture>(aTexture);
    assert(texture);

    assert([m_commandEncoder conformsToProtocol:@protocol(MTLBlitCommandEncoder)]);

    size_t bytesPerPixel = pixelFormatSize(texture->pixelFormat());
    size_t bytesPerRow = bytesPerPixel * texture->width();
    size_t bytesPerImage = bytesPerRow * texture->height();

    assert(bufferOffset + bytesPerImage <= buffer->size());

    [(id<MTLBlitCommandEncoder>)m_commandEncoder copyFromBuffer:buffer->mtlBuffer()
                                                   sourceOffset:bufferOffset
                                              sourceBytesPerRow:bytesPerRow
                                            sourceBytesPerImage:bytesPerImage
                                                     sourceSize:MTLSizeMake(texture->width(), texture->height(), 1)
                                                      toTexture:texture->mtltexture()
                                               destinationSlice:layerIndex
                                               destinationLevel:0
                                              destinationOrigin:MTLOrigin{0, 0, 0}];

    m_usedBuffers.insert(buffer);
    m_usedTextures.insert(texture);
}}

void MetalCommandBuffer::endBlitPass() { @autoreleasepool
{
    assert(m_commandEncoder);
    [m_commandEncoder endEncoding];
    [m_commandEncoder release];
    m_commandEncoder = nil;
}}

void MetalCommandBuffer::presentDrawable(const std::shared_ptr<Drawable>& aDrawable) { @autoreleasepool
{
    auto drawable = std::dynamic_pointer_cast<MetalDrawable>(aDrawable);
    assert(drawable);

    [m_mtlCommandBuffer presentDrawable:drawable->mtlDrawable()];
}}

MetalCommandBuffer::~MetalCommandBuffer() { @autoreleasepool
{
    if (m_sourcePool)
        m_sourcePool->release(this);
    if (m_commandEncoder != nil)
        [m_commandEncoder release];
    if (m_mtlCommandBuffer != nil)
        [m_mtlCommandBuffer release];
}}

MetalCommandBuffer& MetalCommandBuffer::operator = (MetalCommandBuffer&& other) noexcept { @autoreleasepool
{
    if (this != &other)
    {
        if (m_sourcePool)
            m_sourcePool->release(this);
        if (m_commandEncoder != nil)
            [m_commandEncoder release];
        if (m_mtlCommandBuffer != nil)
            [m_mtlCommandBuffer release];

        m_sourcePool = std::exchange(other.m_sourcePool, nullptr);
        m_mtlCommandBuffer = std::exchange(other.m_mtlCommandBuffer, nil);
        m_commandEncoder = std::exchange(other.m_commandEncoder, nil);
        m_usedPipelines = std::move(other.m_usedPipelines);
        m_usedTextures = std::move(other.m_usedTextures);
        m_usedBuffers = std::move(other.m_usedBuffers);
        m_usedSamplers = std::move(other.m_usedSamplers);
        m_usedPBlock = std::move(other.m_usedPBlock);
    }
    return *this;
}}

}
