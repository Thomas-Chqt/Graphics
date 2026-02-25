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

#if !defined(__OBJC__)
#error this file can only by used in objective c
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

    inline uint32_t width() const override { return m_width; }
    inline uint32_t height() const override { return m_height; }
    inline PixelFormat pixelFormat() const override { return m_pixelFormat; };

    std::shared_ptr<Drawable> nextDrawable() override;

    ~MetalSwapchain() override = default;

private:
    uint32_t m_width;
    uint32_t m_height;
    CAMetalLayer* m_mtlLayer;
    PixelFormat m_pixelFormat;

public:
    MetalSwapchain& operator=(const MetalSwapchain&) = delete;
    MetalSwapchain& operator=(MetalSwapchain&&) = delete;
};

} // namespace gfx

#endif // METALSWAPCHAIN_HPP
