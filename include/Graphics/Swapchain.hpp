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

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <cstdint>
    #include <memory>
    namespace ext = std;
#endif

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
    };

public:
    Swapchain(const Swapchain&) = delete;
    Swapchain(Swapchain&&) = delete;

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
