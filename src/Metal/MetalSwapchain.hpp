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
#include "Graphics/Drawable.hpp"

#ifdef __OBJC__
    #import <QuartzCore/CAMetalLayer.h>
#else
    class CAMetalLayer;
#endif // __OBJC__

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    namespace ext = std;
#endif

namespace gfx
{

class MetalDevice;

class MetalSwapchain : public Swapchain
{
public:
    MetalSwapchain(const MetalSwapchain&) = delete;
    MetalSwapchain(MetalSwapchain&&) = delete;

    MetalSwapchain(const MetalDevice&, const Swapchain::Descriptor&);

    ext::shared_ptr<Drawable> nextDrawable(void) override;

    ~MetalSwapchain();

private:
    CAMetalLayer* m_mtlLayer;

public:
    MetalSwapchain& operator=(const MetalSwapchain&) = delete;
    MetalSwapchain& operator=(MetalSwapchain&&) = delete;
};

} // namespace gfx

#endif // METALSWAPCHAIN_HPP
