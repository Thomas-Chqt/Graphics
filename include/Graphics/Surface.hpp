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

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <set>
    namespace ext = std;
#endif

namespace gfx
{

class PhysicalDevice;

class Surface
{
public:
    Surface(const Surface&) = delete;
    Surface(Surface&&) = delete;

    virtual const ext::set<PixelFormat> supportedPixelFormats(const PhysicalDevice&) const = 0;
    virtual const ext::set<PresentMode> supportedPresentModes(const PhysicalDevice&) const = 0;

    virtual ~Surface() = default;

protected:
    Surface() = default;

public:
    Surface& operator=(const Surface&) = delete;
    Surface& operator=(Surface&&) = delete;
};

} // namespace gfx

#endif // SURFACE_HPP
