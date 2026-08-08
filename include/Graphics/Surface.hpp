/*
 * ---------------------------------------------------
 * Surface.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/29 17:48:59
 * ---------------------------------------------------
 */

#ifndef SURFACE_HPP
#define SURFACE_HPP

#include "Graphics/Enums.hpp"

#include <set>
#include <ranges>

namespace gfx
{

class Device;

struct SurfaceFormat
{
    PixelFormat pixelFormat;
    ColorSpace colorSpace;

    bool operator == (const SurfaceFormat&) const = default;
    auto operator <=>(const SurfaceFormat&) const = default;
};

class Surface
{
public:
    Surface(const Surface&) = delete;
    Surface(Surface&&) = delete;

    [[deprecated("use supportedSurfaceFormat")]] inline std::set<PixelFormat> supportedPixelFormats(const Device&) const;

    virtual std::set<SurfaceFormat> supportedSurfaceFormat(const Device&) const = 0;
    virtual std::set<PresentMode> supportedPresentModes(const Device&) const = 0;

    virtual ~Surface() = default;

protected:
    Surface() = default;

public:
    Surface& operator=(const Surface&) = delete;
    Surface& operator=(Surface&&) = delete;
};

inline std::set<PixelFormat> Surface::supportedPixelFormats(const Device& device) const
{
    return supportedSurfaceFormat(device) | std::views::transform(&SurfaceFormat::pixelFormat) | std::ranges::to<std::set>();
}

} // namespace gfx

#endif // SURFACE_HPP
