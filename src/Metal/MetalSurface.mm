/*
 * ---------------------------------------------------
 * MetalSurface.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/30 07:47:06
 * ---------------------------------------------------
 */

#include "Metal/MetalSurface.hpp"

#include <AppKit/NSWindow.h>
#include <QuartzCore/CAMetalLayer.h>

#if defined(GFX_GLFW_ENABLED)
    #include <dlLoad/dlLoad.h>
    class GLFWwindow;
    #define glfwGetCocoaWindow ((id (*)(GLFWwindow*))::getSym(DL_DEFAULT, "glfwGetCocoaWindow"))
#endif

namespace gfx
{

MetalSurface::MetalSurface(GLFWwindow* glfwWindow) { @autoreleasepool
{
    m_mtlLayer = [[CAMetalLayer layer] retain];

    NSWindow* nswindow = glfwGetCocoaWindow(glfwWindow);
    
    nswindow.contentView.layer = m_mtlLayer;
    nswindow.contentView.wantsLayer = YES;
}}

MetalSurface::~MetalSurface() { @autoreleasepool
{
    [m_mtlLayer release];
}}

}
