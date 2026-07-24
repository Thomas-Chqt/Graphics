/*
 * ---------------------------------------------------
 * MetalCommandBuffer.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/08 10:33:10
 * ---------------------------------------------------
 */

#ifndef METALCOMMANDBUFFER_HPP
#define METALCOMMANDBUFFER_HPP

#include "Graphics/CommandBuffer.hpp"
#include "Graphics/GraphicsPipeline.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/ParameterBlock.hpp"
#include "Graphics/Pipeline.hpp"

#include "Metal/MetalBuffer.hpp"
#include "Metal/MetalGraphicsPipeline.hpp"
#include "Metal/MetalComputePipeline.hpp"
#include "Metal/MetalParameterBlock.hpp"
#include "Metal/MetalTexture.hpp"
#include "Metal/MetalSampler.hpp"

#if !defined(__OBJC__)
#error this file can only by used in objective c
#endif

namespace gfx
{

class MetalCommandBufferPool;

class MetalCommandBuffer : public CommandBuffer
{
public:
    MetalCommandBuffer() = default;
    MetalCommandBuffer(const MetalCommandBuffer&) = delete;
    MetalCommandBuffer(MetalCommandBuffer&&) noexcept;

    MetalCommandBuffer(const id<MTLCommandQueue>&);

    void beginRenderPass(RenderPassDescriptor&) override;

    void usePipeline(const std::shared_ptr<const Pipeline>&) override;
    void useVertexBuffer(const std::shared_ptr<Buffer>&) override;

    void setParameterBlock(const std::shared_ptr<const ParameterBlock>&, uint32_t index) override;
    void setPushConstants(const void* data, size_t size) override;

    void drawVertices(uint32_t start, uint32_t count) override;
    void drawIndexedVertices(const std::shared_ptr<Buffer>& idxBuffer) override;

    void endRenderPass() override;

    void beginComputePass(ComputePassDescriptor&) override;
    void dispatchThreadgroups(uint32_t x, uint32_t y = 1, uint32_t z = 1) override;
    void endComputePass() override;


    void beginBlitPass(BlitPassDescriptor&) override;

    void copyBufferToBuffer(const std::shared_ptr<Buffer>& src, const std::shared_ptr<Buffer>& dst, size_t size) override;
    void copyBufferToTexture(const std::shared_ptr<Buffer>& buffer, size_t bufferOffset, const std::shared_ptr<Texture>& texture, uint32_t layerIndex = 0) override;
    void copyTextureToBuffer(const std::shared_ptr<Texture>& texture, uint32_t layerIndex, const std::shared_ptr<Buffer>& buffer, size_t bufferOffset) override;

    void endBlitPass() override;


    void presentDrawable(const std::shared_ptr<Drawable>&) override;

    void addSampledTexture(const std::shared_ptr<Texture>&) override;


    inline id<MTLCommandBuffer> mtlCommandBuffer() const { return m_mtlCommandBuffer; }
    inline id<MTLCommandEncoder> commandEncoder() const { return m_commandEncoder; }

    inline void setSignaledSharedEventValue(uint64_t v) { m_signaledSharedEventValue = v; }
    inline uint64_t signaledSharedEventValue() const { return m_signaledSharedEventValue; }

    ~MetalCommandBuffer() override = default;

private:
    id<MTLCommandBuffer> m_mtlCommandBuffer = nil;
    id<MTLCommandEncoder> m_commandEncoder = nil;

    std::set<std::shared_ptr<const Pipeline>> m_usedPipelines;
    const Pipeline* m_boundPipeline = nullptr;

    std::set<std::shared_ptr<MetalTexture>> m_usedTextures;
    std::set<std::shared_ptr<MetalBuffer>> m_usedBuffers;
    std::set<std::shared_ptr<MetalSampler>> m_usedSamplers;

    std::set<std::shared_ptr<const MetalParameterBlock>> m_usedPBlock;

    uint64_t m_signaledSharedEventValue = 0;

public:
    MetalCommandBuffer& operator = (const MetalCommandBuffer&) = delete;
    MetalCommandBuffer& operator = (MetalCommandBuffer&&) noexcept;
};

}

#endif // METALCOMMANDBUFFER_HPP
