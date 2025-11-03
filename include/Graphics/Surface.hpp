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

namespace gfx
{

class Device;

class Surface
{
public:
    Surface(const Surface&) = delete;
    Surface(Surface&&) = delete;

    virtual const std::set<PixelFormat> supportedPixelFormats(const Device&) const = 0;
    virtual const std::set<PresentMode> supportedPresentModes(const Device&) const = 0;

    virtual ~Surface() = default;

protected:
    Surface() = default;

public:
    Surface& operator=(const Surface&) = delete;
    Surface& operator=(Surface&&) = delete;
};

} // namespace gfx

#endif // SURFACE_HPP
