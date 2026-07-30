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
#include "Graphics/GraphicsPipeline.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/ParameterBlock.hpp"

#include "Metal/MetalDevice.hpp"
#include "Metal/MetalCommandBuffer.hpp"
#include "Metal/MetalBuffer.hpp"
#include "Metal/MetalSampler.hpp"
#include "Metal/MetalTexture.hpp"
#include "Metal/MetalPassDescriptor.hpp"
#include "Metal/MetalGraphicsPipeline.hpp"
#include "Metal/MetalComputePipeline.hpp"
#include "Metal/MetalParameterBlock.hpp"
#include "Metal/MetalDrawable.hpp"
#include "Metal/MetalCommandBufferPool.hpp"

#import "Metal/MetalEnums.hpp"

namespace gfx
{

MetalCommandBuffer::MetalCommandBuffer(MetalCommandBuffer&& other) noexcept
    : CommandBuffer(std::move(other)),
      m_mtlCommandBuffer(std::exchange(other.m_mtlCommandBuffer, nil)),
      m_commandEncoder(std::exchange(other.m_commandEncoder, nil)),
      m_usedPipelines(std::move(other.m_usedPipelines)),
      m_boundPipeline(std::exchange(other.m_boundPipeline, nullptr)),
      m_usedTextures(std::move(other.m_usedTextures)),
      m_usedBuffers(std::move(other.m_usedBuffers)),
      m_usedSamplers(std::move(other.m_usedSamplers)),
      m_usedPBlock(std::move(other.m_usedPBlock))
{
}

MetalCommandBuffer::MetalCommandBuffer(const id<MTLCommandQueue>& queue) { @autoreleasepool
{
    m_mtlCommandBuffer = [queue commandBuffer];
}}

void MetalCommandBuffer::beginRenderPass(RenderPassDescriptor& descriptor) { @autoreleasepool
{
    assert(m_commandEncoder == nil);
    assert(m_boundPipeline == nullptr);

    auto* metalDescriptor = dynamic_cast<MetalRenderPassDescriptor*>(&descriptor);
    assert(metalDescriptor);

    for (const auto& colorAttachment : descriptor.colorAttachments())
    {
        auto texture = std::dynamic_pointer_cast<MetalTexture>(colorAttachment.texture);
        assert(texture);
        assert(texture->type() == TextureType::texture2d);
        assert(texture->mipLevelCount() == 1);
        m_usedTextures.insert(texture);
    }

    if (const auto& depthAttachment = descriptor.depthAttachment())
    {
        auto texture = std::dynamic_pointer_cast<MetalTexture>(depthAttachment->texture);
        assert(texture);
        assert(texture->type() == TextureType::texture2d);
        assert(texture->mipLevelCount() == 1);
        m_usedTextures.insert(texture);
    }

    m_commandEncoder = [m_mtlCommandBuffer renderCommandEncoderWithDescriptor:metalDescriptor->mtlRenderPassDescriptor()];
}}

void MetalCommandBuffer::usePipeline(const std::shared_ptr<const Pipeline>& pipeline) { @autoreleasepool
{
    if (auto graphicsPipeline = std::dynamic_pointer_cast<const MetalGraphicsPipeline>(pipeline))
    {
        assert([m_commandEncoder conformsToProtocol:@protocol(MTLRenderCommandEncoder)]);
        auto renderCommandEncoder = (id<MTLRenderCommandEncoder>)m_commandEncoder;
        [renderCommandEncoder setRenderPipelineState:graphicsPipeline->renderPipelineState()];
        [renderCommandEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
        [renderCommandEncoder setCullMode:toMTLCullMode(graphicsPipeline->cullMode())];
        if (graphicsPipeline->depthStencilState() != nil)
            [renderCommandEncoder setDepthStencilState:graphicsPipeline->depthStencilState()];
    }
    else if (auto computePipeline = std::dynamic_pointer_cast<const MetalComputePipeline>(pipeline))
    {
        assert([m_commandEncoder conformsToProtocol:@protocol(MTLComputeCommandEncoder)]);
        auto computeCommandEncoder = (id<MTLComputeCommandEncoder>)m_commandEncoder;
        [computeCommandEncoder setComputePipelineState:computePipeline->computePipelineState()];
    }
    else {
        std::unreachable();
    }

    m_usedPipelines.insert(pipeline);
    m_boundPipeline = pipeline.get();
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
    assert(pBlock);

    if ([m_commandEncoder conformsToProtocol:@protocol(MTLRenderCommandEncoder)])
    {
        auto renderCommandEncoder = (id<MTLRenderCommandEncoder>)m_commandEncoder;

        for (const auto& encodedBuffer : pBlock->encodedBuffers())
            [renderCommandEncoder useResource:encodedBuffer.resource->mtlBuffer() usage:toMTLResourceUsage(encodedBuffer.binding.usages) stages:toMTLRenderStages(encodedBuffer.binding.usages)];

        for (const auto& encodedTexture : pBlock->encodedTextures())
            [renderCommandEncoder useResource:encodedTexture.resource->mtltexture() usage:toMTLResourceUsage(encodedTexture.binding.usages) stages:toMTLRenderStages(encodedTexture.binding.usages)];

        const bool hasVertexBindings =
            std::ranges::any_of(pBlock->encodedBuffers(),  [](const auto& encodedBuffer)  { return encodedBuffer.binding.usages  & BindingUsage::vertexRead || encodedBuffer.binding.usages & BindingUsage::vertexWrite;  }) ||
            std::ranges::any_of(pBlock->encodedTextures(), [](const auto& encodedTexture) { return encodedTexture.binding.usages & BindingUsage::vertexRead || encodedTexture.binding.usages & BindingUsage::vertexWrite; }) ||
            std::ranges::any_of(pBlock->encodedSamplers(), [](const auto& encodedSampler) { return encodedSampler.binding.usages & BindingUsage::vertexRead || encodedSampler.binding.usages & BindingUsage::vertexWrite; });

        const bool hasFragmentBindings =
            std::ranges::any_of(pBlock->encodedBuffers(),  [](const auto& encodedBuffer)  { return encodedBuffer.binding.usages & BindingUsage::fragmentRead  || encodedBuffer.binding.usages & BindingUsage::fragmentWrite;  }) ||
            std::ranges::any_of(pBlock->encodedTextures(), [](const auto& encodedTexture) { return encodedTexture.binding.usages & BindingUsage::fragmentRead || encodedTexture.binding.usages & BindingUsage::fragmentWrite; }) ||
            std::ranges::any_of(pBlock->encodedSamplers(), [](const auto& encodedSampler) { return encodedSampler.binding.usages & BindingUsage::fragmentRead || encodedSampler.binding.usages & BindingUsage::fragmentWrite; });

        assert(hasVertexBindings || hasFragmentBindings);

        if (hasVertexBindings)
            [renderCommandEncoder setVertexBuffer:pBlock->argumentBuffer().mtlBuffer() offset:pBlock->offset() atIndex:index];

        if (hasFragmentBindings)
            [renderCommandEncoder setFragmentBuffer:pBlock->argumentBuffer().mtlBuffer() offset:pBlock->offset() atIndex:index];
    }
    else if ([m_commandEncoder conformsToProtocol:@protocol(MTLComputeCommandEncoder)])
    {
        auto computeCommandEncoder = (id<MTLComputeCommandEncoder>)m_commandEncoder;

        for (const auto& encodedBuffer : pBlock->encodedBuffers())
            [computeCommandEncoder useResource:encodedBuffer.resource->mtlBuffer() usage:toMTLResourceUsage(encodedBuffer.binding.usages)];

        for (const auto& encodedTexture : pBlock->encodedTextures())
            [computeCommandEncoder useResource:encodedTexture.resource->mtltexture() usage:toMTLResourceUsage(encodedTexture.binding.usages)];

        const bool hasComputeBindings =
            std::ranges::any_of(pBlock->encodedBuffers(),  [](const auto& encodedBuffer)  { return encodedBuffer.binding.usages & BindingUsage::computeRead  || encodedBuffer.binding.usages & BindingUsage::computeWrite;  }) ||
            std::ranges::any_of(pBlock->encodedTextures(), [](const auto& encodedTexture) { return encodedTexture.binding.usages & BindingUsage::computeRead || encodedTexture.binding.usages & BindingUsage::computeWrite; }) ||
            std::ranges::any_of(pBlock->encodedSamplers(), [](const auto& encodedSampler) { return encodedSampler.binding.usages & BindingUsage::computeRead || encodedSampler.binding.usages & BindingUsage::computeWrite; });
        assert(hasComputeBindings);
        [computeCommandEncoder setBuffer:pBlock->argumentBuffer().mtlBuffer() offset:pBlock->offset() atIndex:index];
    }
    else {
        std::unreachable();
    }

    m_usedBuffers.insert_range(pBlock->encodedBuffers()   | std::views::transform([](const auto& encodedBuffer)  -> std::shared_ptr<MetalBuffer>  { return encodedBuffer.resource;  }));
    m_usedTextures.insert_range(pBlock->encodedTextures() | std::views::transform([](const auto& encodedTexture) -> std::shared_ptr<MetalTexture> { return encodedTexture.resource; }));
    m_usedSamplers.insert_range(pBlock->encodedSamplers() | std::views::transform([](const auto& encodedSampler) -> std::shared_ptr<MetalSampler> { return encodedSampler.resource; }));
    m_usedPBlock.insert(pBlock);
}}

void MetalCommandBuffer::setPushConstants(const void* data, size_t size) { @autoreleasepool
{
    assert(size <= 128);
    if ([m_commandEncoder conformsToProtocol:@protocol(MTLRenderCommandEncoder)])
    {
        auto renderCommandEncoder = (id<MTLRenderCommandEncoder>)m_commandEncoder;
        [renderCommandEncoder setVertexBytes:data length:size atIndex:6];
        [renderCommandEncoder setFragmentBytes:data length:size atIndex:6];
    }
    else if ([m_commandEncoder conformsToProtocol:@protocol(MTLComputeCommandEncoder)])
    {
        auto computeCommandEncoder = (id<MTLComputeCommandEncoder>)m_commandEncoder;
        [computeCommandEncoder setBytes:data length:size atIndex:6];
    }
    else {
        std::unreachable();
    }
}}

void MetalCommandBuffer::drawVertices(uint32_t start, uint32_t count) { @autoreleasepool
{
    assert([m_commandEncoder conformsToProtocol:@protocol(MTLRenderCommandEncoder)]);
    auto renderCommandEncoder = (id<MTLRenderCommandEncoder>)m_commandEncoder;

    assert(dynamic_cast<const MetalGraphicsPipeline*>(m_boundPipeline));

    [renderCommandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:start vertexCount:count];
}}

void MetalCommandBuffer::drawIndexedVertices(const std::shared_ptr<Buffer>& buffer) { @autoreleasepool
{
    auto idxBuffer = std::dynamic_pointer_cast<MetalBuffer>(buffer);
    assert(idxBuffer);

    assert([m_commandEncoder conformsToProtocol:@protocol(MTLRenderCommandEncoder)]);
    auto renderCommandEncoder = (id<MTLRenderCommandEncoder>)m_commandEncoder;

    assert(dynamic_cast<const MetalGraphicsPipeline*>(m_boundPipeline));

    [renderCommandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                     indexCount:idxBuffer->size() / sizeof(uint32_t)
                                      indexType:MTLIndexTypeUInt32
                                    indexBuffer:idxBuffer->mtlBuffer()
                              indexBufferOffset:0];

    m_usedBuffers.insert(idxBuffer);
}}

void MetalCommandBuffer::endRenderPass() { @autoreleasepool
{
    assert(m_commandEncoder);
    [m_commandEncoder endEncoding];
    m_boundPipeline = nullptr;
    m_commandEncoder = nil;
}}

void MetalCommandBuffer::beginComputePass(ComputePassDescriptor& descriptor) { @autoreleasepool
{
    assert(m_commandEncoder == nil);
    assert(m_boundPipeline == nullptr);

    auto* metalDescriptor = dynamic_cast<MetalComputePassDescriptor*>(&descriptor);
    assert(metalDescriptor);

    m_commandEncoder = [m_mtlCommandBuffer computeCommandEncoderWithDescriptor:metalDescriptor->mtlComputePassDescriptor()];
    assert(m_commandEncoder);
}}

void MetalCommandBuffer::dispatchThreadgroups(uint32_t x, uint32_t y, uint32_t z) { @autoreleasepool
{
    assert(x > 0 && y > 0 && z > 0);
    assert([m_commandEncoder conformsToProtocol:@protocol(MTLComputeCommandEncoder)]);
    assert(m_boundPipeline);
    const auto* computePipeline = dynamic_cast<const MetalComputePipeline*>(m_boundPipeline);
    assert(computePipeline);
    [(id<MTLComputeCommandEncoder>)m_commandEncoder dispatchThreadgroups:MTLSizeMake(x, y, z)
                                                   threadsPerThreadgroup:computePipeline->threadsPerThreadgroup()];
}}

void MetalCommandBuffer::endComputePass() { @autoreleasepool
{
    assert([m_commandEncoder conformsToProtocol:@protocol(MTLComputeCommandEncoder)]);
    [m_commandEncoder endEncoding];
    m_commandEncoder = nil;
    m_boundPipeline = nullptr;
}}

void MetalCommandBuffer::beginBlitPass(BlitPassDescriptor& descriptor) { @autoreleasepool
{
    assert(m_commandEncoder == nil);
    assert(m_boundPipeline == nullptr);

    auto* metalDescriptor = dynamic_cast<MetalBlitPassDescriptor*>(&descriptor);
    assert(metalDescriptor);
    m_commandEncoder = [m_mtlCommandBuffer blitCommandEncoderWithDescriptor:metalDescriptor->mtlBlitPassDescriptor()];
}}

void MetalCommandBuffer::copyBufferToBuffer(const std::shared_ptr<Buffer>& aSrc, const std::shared_ptr<Buffer>& aDst, size_t size) { @autoreleasepool
{
    auto src = std::dynamic_pointer_cast<MetalBuffer>(aSrc);
    assert(src);

    auto dst = std::dynamic_pointer_cast<MetalBuffer>(aDst);
    assert(dst);

    assert(src->usages() & BufferUsage::copySource && dst->usages() & BufferUsage::copyDestination);
    assert(size <= src->size());
    assert(size <= dst->size());

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

    assert(buffer->usages() & BufferUsage::copySource);
    assert(texture->usages() & TextureUsage::copyDestination);
    assert(layerIndex < (texture->type() == TextureType::textureCube ? 6u : texture->arrayLayerCount()));
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

void MetalCommandBuffer::copyTextureToBuffer(const std::shared_ptr<Texture>& aTexture, uint32_t layerIndex, const std::shared_ptr<Buffer>& aBuffer, size_t bufferOffset) { @autoreleasepool
{
    auto texture = std::dynamic_pointer_cast<MetalTexture>(aTexture);
    assert(texture);

    auto buffer = std::dynamic_pointer_cast<MetalBuffer>(aBuffer);
    assert(buffer);

    assert(texture->usages() & TextureUsage::copySource);
    assert(buffer->usages() & BufferUsage::copyDestination);
    assert(layerIndex < (texture->type() == TextureType::textureCube ? 6u : texture->arrayLayerCount()));
    assert([m_commandEncoder conformsToProtocol:@protocol(MTLBlitCommandEncoder)]);

    size_t bytesPerPixel = pixelFormatSize(texture->pixelFormat());
    size_t bytesPerRow = bytesPerPixel * texture->width();
    size_t bytesPerImage = bytesPerRow * texture->height();

    assert(bufferOffset + bytesPerImage <= buffer->size());

    [(id<MTLBlitCommandEncoder>)m_commandEncoder copyFromTexture:texture->mtltexture()
                                                     sourceSlice:layerIndex
                                                     sourceLevel:0
                                                    sourceOrigin:MTLOrigin{0, 0, 0}
                                                      sourceSize:MTLSizeMake(texture->width(), texture->height(), 1)
                                                        toBuffer:buffer->mtlBuffer()
                                               destinationOffset:bufferOffset
                                          destinationBytesPerRow:bytesPerRow
                                        destinationBytesPerImage:bytesPerImage];

    m_usedTextures.insert(texture);
    m_usedBuffers.insert(buffer);
}}

void MetalCommandBuffer::endBlitPass() { @autoreleasepool
{
    assert(m_commandEncoder);
    [m_commandEncoder endEncoding];
    m_commandEncoder = nil;
    m_boundPipeline = nullptr;
}}

void MetalCommandBuffer::generateMipmaps(const std::shared_ptr<Texture>& aTexture) { @autoreleasepool
{
    auto texture = std::dynamic_pointer_cast<MetalTexture>(aTexture);
    assert(texture);
    assert(texture->mipLevelCount() > 1);
    assert(texture->usages() & TextureUsage::copySource);
    assert(texture->usages() & TextureUsage::copyDestination);
    assert(texture->pixelFormat() != PixelFormat::RG32Uint);
    assert(texture->pixelFormat() != PixelFormat::Depth32Float);
    assert([m_commandEncoder conformsToProtocol:@protocol(MTLBlitCommandEncoder)]);

    [(id<MTLBlitCommandEncoder>)m_commandEncoder generateMipmapsForTexture:texture->mtltexture()];
    m_usedTextures.insert(texture);
}}

void MetalCommandBuffer::presentDrawable(const std::shared_ptr<Drawable>& aDrawable) { @autoreleasepool
{
    auto drawable = std::dynamic_pointer_cast<MetalDrawable>(aDrawable);
    assert(drawable);

    assert(m_boundPipeline == nullptr);

    [m_mtlCommandBuffer presentDrawable:drawable->mtlDrawable()];
}}

void MetalCommandBuffer::addSampledTexture(const std::shared_ptr<Texture>& aTexture)
{
    auto texture = std::dynamic_pointer_cast<MetalTexture>(aTexture);
    assert(texture);
    m_usedTextures.insert(texture);
}

MetalCommandBuffer& MetalCommandBuffer::operator = (MetalCommandBuffer&& other) noexcept { @autoreleasepool
{
    if (this != &other)
    {
        m_mtlCommandBuffer = std::exchange(other.m_mtlCommandBuffer, nil);
        m_commandEncoder = std::exchange(other.m_commandEncoder, nil);
        m_usedPipelines = std::move(other.m_usedPipelines);
        m_boundPipeline = std::exchange(other.m_boundPipeline, nullptr);
        m_usedTextures = std::move(other.m_usedTextures);
        m_usedBuffers = std::move(other.m_usedBuffers);
        m_usedSamplers = std::move(other.m_usedSamplers);
        m_usedPBlock = std::move(other.m_usedPBlock);
    }
    return *this;
}}

}
