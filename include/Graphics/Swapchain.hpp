/*
 * ---------------------------------------------------
 * Swapchain.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/01 13:26:29
 * ---------------------------------------------------
 */

#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include "Graphics/Surface.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Drawable.hpp"
#include "Graphics/Texture.hpp"

#include <cstdint>
#include <memory>

namespace gfx
{

class Swapchain
{
public:
    struct Descriptor
    {
        Surface* surface = nullptr;
        uint32_t width = 0, height = 0;
        uint32_t imageCount = 3;
        uint32_t drawableCount = 3;
        PixelFormat pixelFormat = PixelFormat::BGRA8Unorm;
        PresentMode presentMode = PresentMode::fifo;
        auto operator<=>(const Descriptor&) const = default;
    };

public:
    Swapchain(const Swapchain&) = delete;
    Swapchain(Swapchain&&) = delete;

    // descriptor used to create textures for drawables
    virtual const Texture::Descriptor& drawablesTextureDescriptor() const = 0;

    virtual std::shared_ptr<Drawable> nextDrawable() = 0;

    virtual ~Swapchain() = default;

protected:
    Swapchain() = default;

public:
    Swapchain& operator=(const Swapchain&) = delete;
    Swapchain& operator=(Swapchain&&) = delete;
};

} // namespace gfx

#endif // SWAPCHAIN_HPP
