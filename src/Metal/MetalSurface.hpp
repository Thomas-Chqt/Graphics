/*
 * ---------------------------------------------------
 * MetalSurface.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/30 07:44:04
 * ---------------------------------------------------
 */

#ifndef METALSURFACE_HPP
#define METALSURFACE_HPP

#include "Graphics/Surface.hpp"
#include "Graphics/Enums.hpp"

#ifdef __OBJC__
    #import <QuartzCore/CAMetalLayer.h>
#else
    class CAMetalLayer;
#endif // __OBJC__

#if defined(GFX_GLFW_ENABLED)
    class GLFWwindow;
#endif

namespace gfx
{

class MetalSurface : public Surface
{
public:
    MetalSurface() = delete;
    MetalSurface(const MetalSurface&) = delete;
    MetalSurface(MetalSurface&&) = delete;

#if defined(GFX_GLFW_ENABLED)
    MetalSurface(GLFWwindow*);
#endif

    const ext::set<PixelFormat> supportedPixelFormats(const Device&) const override;
    const ext::set<PresentMode> supportedPresentModes(const Device&) const override;

    CAMetalLayer* mtlLayer() const { return m_mtlLayer; }

    ~MetalSurface();

private:
    CAMetalLayer* m_mtlLayer;

public:
    MetalSurface& operator=(const MetalSurface&) = delete;
    MetalSurface& operator=(MetalSurface&&) = delete;
};

} // namespace gfx

#endif // METALSURFACE_HPP
