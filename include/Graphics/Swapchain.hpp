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

#include "Graphics/RenderPass.hpp"
#include "Graphics/Surface.hpp"
#include "Graphics/Enums.hpp"

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <cstdint>
    namespace ext = std;
#endif

namespace gfx
{

class Swapchain
{
public:
    struct Descriptor
    {
        Surface*/*not null*/ surface;
        uint32_t width, height;
        uint32_t imageCount = 3;
        PixelFormat pixelFormat = PixelFormat::BGRA8Unorm;
        PresentMode presentMode = PresentMode::fifo;
        RenderPass*/*not null*/ renderPass;
    };

public:
    Swapchain(const Swapchain&) = delete;
    Swapchain(Swapchain&&) = delete;

    virtual ~Swapchain() = default;

protected:
    Swapchain() = default;

public:
    Swapchain& operator=(const Swapchain&) = delete;
    Swapchain& operator=(Swapchain&&) = delete;
};

} // namespace gfx

#endif // SWAPCHAIN_HPP
