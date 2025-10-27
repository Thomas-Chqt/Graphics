/*
 * ---------------------------------------------------
 * MetalSurface.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/30 07:47:06
 * ---------------------------------------------------
 */

#include "Graphics/Enums.hpp"

#include "Metal/MetalSurface.hpp"

namespace gfx
{

#if defined(GFX_GLFW_ENABLED)
MetalSurface::MetalSurface(GLFWwindow* glfwWindow) { @autoreleasepool
{
    m_mtlLayer = [[CAMetalLayer layer] retain];

    NSWindow* nswindow = glfwGetCocoaWindow(glfwWindow); // NOLINT
    nswindow.contentView.layer = m_mtlLayer;
    nswindow.contentView.wantsLayer = YES;
}}
#endif

const std::set<PixelFormat> MetalSurface::supportedPixelFormats(const Device&) const
{
    return {
        PixelFormat::BGRA8Unorm,
        PixelFormat::BGRA8Unorm_sRGB,
    };
}

const std::set<PresentMode> MetalSurface::supportedPresentModes(const Device&) const
{
    return {
        PresentMode::fifo
    };
}

MetalSurface::~MetalSurface() { @autoreleasepool
{
    [m_mtlLayer release];
}}

}
