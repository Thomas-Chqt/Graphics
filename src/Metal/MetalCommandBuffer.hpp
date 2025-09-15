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
#include "Graphics/Framebuffer.hpp"
#include "Graphics/GraphicsPipeline.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/ParameterBlock.hpp"

#include "Metal/MetalBuffer.hpp"
#include "Metal/MetalDrawable.hpp"
#include "Metal/MetalGraphicsPipeline.hpp"
#include "Metal/MetalParameterBlock.hpp"
#include "Metal/MetalTexture.hpp"
#include "Metal/MetalSampler.hpp"

namespace gfx
{

class MetalCommandBufferPool;

class MetalCommandBuffer : public CommandBuffer
{
public:
    MetalCommandBuffer() = default;
    MetalCommandBuffer(const MetalCommandBuffer&) = delete;
    MetalCommandBuffer(MetalCommandBuffer&&) noexcept;

    MetalCommandBuffer(const id<MTLCommandQueue>&, MetalCommandBufferPool*);

    CommandBufferPool* pool() override;

    void beginRenderPass(const Framebuffer&) override;

    void usePipeline(const ext::shared_ptr<const GraphicsPipeline>&) override;
    void useVertexBuffer(const ext::shared_ptr<Buffer>&) override;

    void setParameterBlock(const ext::shared_ptr<const ParameterBlock>&, uint32_t index) override;

    void drawVertices(uint32_t start, uint32_t count) override;
    void drawIndexedVertices(const ext::shared_ptr<Buffer>& idxBuffer) override;

#if defined(GFX_IMGUI_ENABLED)
    void imGuiRenderDrawData(ImDrawData*) const override;
#endif

    void endRenderPass() override;


    void beginBlitPass() override;

    void copyBufferToBuffer(const ext::shared_ptr<Buffer>& src, const ext::shared_ptr<Buffer>& dst, size_t size) override;
    void copyBufferToTexture(const ext::shared_ptr<Buffer>& buffer, size_t bufferOffset, const ext::shared_ptr<Texture>& texture, uint32_t layerIndex = 0) override;

    void endBlitPass() override;


    void presentDrawable(const ext::shared_ptr<Drawable>&) override;


    inline const id<MTLCommandBuffer>& mtlCommandBuffer() const { return m_mtlCommandBuffer; }
    inline const id<MTLCommandEncoder>& commandEncoder() const { return m_commandEncoder; }
    inline void clearSourcePool() { m_sourcePool = nullptr; }

    ~MetalCommandBuffer() override;

private:
    MetalCommandBufferPool* m_sourcePool = nullptr;

    id<MTLCommandBuffer> m_mtlCommandBuffer = nil;
    id<MTLCommandEncoder> m_commandEncoder = nil;

    ext::set<ext::shared_ptr<const MetalGraphicsPipeline>> m_usedPipelines;

    ext::set<ext::shared_ptr<MetalTexture>> m_usedTextures;
    ext::set<ext::shared_ptr<MetalBuffer>> m_usedBuffers;
    ext::set<ext::shared_ptr<MetalSampler>> m_usedSamplers;

    ext::set<ext::shared_ptr<const MetalParameterBlock>> m_usedPBlock;

public:
    MetalCommandBuffer& operator = (const MetalCommandBuffer&) = delete;
    MetalCommandBuffer& operator = (MetalCommandBuffer&&) noexcept;
};

}

#endif // METALCOMMANDBUFFER_HPP
