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
#include "Metal/MetalDrawable.hpp"
#include "Metal/MetalTexture.hpp"

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

    inline const Texture::Descriptor& drawablesTextureDescriptor() const override { return m_swapchainImagesDescriptor; }

    std::shared_ptr<Drawable> nextDrawable() override;

    ~MetalSwapchain() override = default;

private:
    Texture::Descriptor m_swapchainImagesDescriptor;

    CAMetalLayer* m_mtlLayer;
    std::vector<std::shared_ptr<MetalDrawable>> m_drawables;
    uint32_t m_nextDrawableIndex = 0;

public:
    MetalSwapchain& operator=(const MetalSwapchain&) = delete;
    MetalSwapchain& operator=(MetalSwapchain&&) = delete;
};

} // namespace gfx

#endif // METALSWAPCHAIN_HPP
