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

#include <AppKit/NSWindow.h>
#include <QuartzCore/CAMetalLayer.h>

#if defined(GFX_GLFW_ENABLED)
    #include <dlLoad/dlLoad.h>
    struct GLFWwindow;
    #define glfwGetCocoaWindow ((id (*)(GLFWwindow*))::getSym(DL_DEFAULT, "glfwGetCocoaWindow"))
#endif

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

const ext::set<PixelFormat> MetalSurface::supportedPixelFormats(const Device&) const
{
    return {
        PixelFormat::BGRA8Unorm,
        PixelFormat::BGRA8Unorm_sRGB,
    };
}

const ext::set<PresentMode> MetalSurface::supportedPresentModes(const Device&) const
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
