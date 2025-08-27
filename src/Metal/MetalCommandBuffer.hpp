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
#include "Metal/MetalGraphicsPipeline.hpp"
#include "Metal/MetalTexture.hpp"

namespace gfx
{

class MetalCommandBuffer : public CommandBuffer
{
public:
    MetalCommandBuffer() = default;
    MetalCommandBuffer(const MetalCommandBuffer&) = delete;
    MetalCommandBuffer(MetalCommandBuffer&&) noexcept;

    MetalCommandBuffer(const id<MTLCommandQueue>&);

    void beginRenderPass(const Framebuffer&) override;

    void usePipeline(const ext::shared_ptr<const GraphicsPipeline>&) override;
    void useVertexBuffer(const ext::shared_ptr<Buffer>&) override;

    void setParameterBlock(const ParameterBlock&, uint32_t index) override;

    void drawVertices(uint32_t start, uint32_t count) override;
    void drawIndexedVertices(const ext::shared_ptr<Buffer>& idxBuffer) override;

#if defined(GFX_IMGUI_ENABLED)
    void imGuiRenderDrawData(ImDrawData*) const override;
#endif

    void endRenderPass() override;

    const id<MTLCommandBuffer>& mtlCommandBuffer() const { return m_mtlCommandBuffer; }

    ~MetalCommandBuffer() override;

private:
    id<MTLCommandBuffer> m_mtlCommandBuffer = nil;
    id<MTLCommandEncoder> m_commandEncoder = nil;

    ext::set<ext::shared_ptr<const MetalGraphicsPipeline>> m_usedPipelines;
    ext::set<ext::shared_ptr<const MetalTexture>> m_usedTextures;
    ext::set<ext::shared_ptr<const MetalBuffer>> m_usedBuffers;

public:
    MetalCommandBuffer& operator = (const MetalCommandBuffer&) = delete;
    MetalCommandBuffer& operator = (MetalCommandBuffer&&) noexcept;
};

}

#endif // METALCOMMANDBUFFER_HPP
