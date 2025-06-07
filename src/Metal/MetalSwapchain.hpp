/*
 * ---------------------------------------------------
 * MetalSwapchain.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/01 19:06:54
 * ---------------------------------------------------
 */

#ifndef METALSWAPCHAIN_HPP
#define METALSWAPCHAIN_HPP

#include "Graphics/Swapchain.hpp"

#include "Metal/MetalDevice.hpp"
#include "Metal/MetalTexture.hpp"
#include "Metal/MetalFramebuffer.hpp"

#ifdef __OBJC__
    #import <QuartzCore/CAMetalLayer.h>
#else
    template<typename T> using id = T*;
    class CAMetalLayer;
    class CAMetalDrawable;
#endif // __OBJC__

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    #include <vector>
    namespace ext = std;
#endif

namespace gfx
{

class MetalSwapchain : public Swapchain
{
public:
    MetalSwapchain(const MetalSwapchain&) = delete;
    MetalSwapchain(MetalSwapchain&&) = delete;

    MetalSwapchain(const MetalDevice&, const Swapchain::Descriptor&);

    const Framebuffer& nextFrameBuffer(void) override;

    ~MetalSwapchain();

private:
    CAMetalLayer* m_mtlLayer;
    ext::vector<ext::shared_ptr<MetalTexture>> m_depthAttachments;
    
    int m_drawableIndex = 0;
    id<CAMetalDrawable> m_currentDrawable;
    MetalFramebuffer m_currentFramebuffer;

public:
    MetalSwapchain& operator=(const MetalSwapchain&) = delete;
    MetalSwapchain& operator=(MetalSwapchain&&) = delete;
};

} // namespace gfx

#endif // METALSWAPCHAIN_HPP
