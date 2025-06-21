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

#include "Metal/MetalFramebuffer.hpp"
#include "Metal/MetalRenderPass.hpp"

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
//
#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    #include <vector>
    namespace ext = std;
#endif

namespace gfx
{

class MetalCommandBuffer : public CommandBuffer
{
public:
    MetalCommandBuffer() = default;
    MetalCommandBuffer(const MetalCommandBuffer&) = delete;
    MetalCommandBuffer(MetalCommandBuffer&&);

    MetalCommandBuffer(id<MTLCommandQueue>);
    
    void beginRenderPass(const ext::shared_ptr<RenderPass>&, const ext::shared_ptr<Framebuffer>&) override;
    void endRenderPass(void) override;

    id<MTLCommandBuffer> mtlCommandBuffer() { return m_mtlCommandBuffer; }

    ~MetalCommandBuffer();

private:
    id<MTLCommandBuffer> m_mtlCommandBuffer = nil;
    id<MTLRenderCommandEncoder> m_commandEncoder = nil;

    ext::vector<ext::shared_ptr<MetalRenderPass>> m_usedRenderPasses;
    ext::vector<ext::shared_ptr<MetalFramebuffer>> m_usedFramebuffers;

public:
    MetalCommandBuffer& operator = (const MetalCommandBuffer&) = delete;
    MetalCommandBuffer& operator = (MetalCommandBuffer&&);
};

}

#endif // METALCOMMANDBUFFER_HPP
