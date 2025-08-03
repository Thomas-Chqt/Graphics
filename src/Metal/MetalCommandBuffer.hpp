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

#include "Metal/MetalBuffer.hpp"
#include "Metal/MetalGraphicsPipeline.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    #define nil nullptr
    template<typename T>
    using id = T*;
    class MTLCommandQueue;
    class MTLCommandBuffer;
    class MTLRenderCommandEncoder;
#endif // __OBJC__

#if defined(GFX_USE_UTILSCPP)
#else
    #include <memory>
    #include <set>
    namespace ext = std;
#endif

#if defined(GFX_IMGUI_ENABLED)
    struct ImDrawData;
#endif

namespace gfx
{

class MetalCommandBuffer : public CommandBuffer
{
public:
    MetalCommandBuffer() = default;
    MetalCommandBuffer(const MetalCommandBuffer&) = delete;
    MetalCommandBuffer(MetalCommandBuffer&&) = delete;

    MetalCommandBuffer(const id<MTLCommandQueue>&);
    
    void beginRenderPass(const Framebuffer&) override;

    void usePipeline(const ext::shared_ptr<const GraphicsPipeline>&) override;
    void useVertexBuffer(const ext::shared_ptr<const Buffer>&) override;

    void drawVertices(uint32_t start, uint32_t count) override;

#if defined(GFX_IMGUI_ENABLED)
    void imGuiRenderDrawData(ImDrawData*) const override;
#endif

    void endRenderPass(void) override;

    const id<MTLCommandBuffer>& mtlCommandBuffer() const { return m_mtlCommandBuffer; }

    ~MetalCommandBuffer();

private:
    id<MTLCommandBuffer> m_mtlCommandBuffer = nil;
    id<MTLRenderCommandEncoder> m_commandEncoder = nil;

    ext::set<ext::shared_ptr<const MetalGraphicsPipeline>> m_usedPipelines;
    ext::set<ext::shared_ptr<const MetalBuffer>> m_usedBuffers;

public:
    MetalCommandBuffer& operator = (const MetalCommandBuffer&) = delete;
    MetalCommandBuffer& operator = (MetalCommandBuffer&&) = delete;
};

}

#endif // METALCOMMANDBUFFER_HPP
