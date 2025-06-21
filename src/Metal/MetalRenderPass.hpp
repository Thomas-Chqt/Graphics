/*
 * ---------------------------------------------------
 * MetalRenderPass.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/04 13:07:46
 * ---------------------------------------------------
 */

#ifndef METALRENDERPASS_HPP
#define METALRENDERPASS_HPP

#include "Graphics/RenderPass.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    class MTLRenderPassDescriptor;
#endif // __OBJC__

namespace gfx
{

class MetalRenderPass : public RenderPass
{
public:
    MetalRenderPass(const MetalRenderPass&) = delete;
    MetalRenderPass(MetalRenderPass&&)      = delete;

    MetalRenderPass(const RenderPass::Descriptor&);

    inline const MTLRenderPassDescriptor* mtlRenderPassDescriptor() const { return m_mtlRenderPassDescriptor; }
    
    ~MetalRenderPass();

private:
    MTLRenderPassDescriptor* m_mtlRenderPassDescriptor;

public:
    MetalRenderPass& operator = (const MetalRenderPass&) = delete;
    MetalRenderPass& operator = (MetalRenderPass&&)      = delete;
};

}

#endif // METALRENDERPASS_HPP
