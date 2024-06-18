/*
 * ---------------------------------------------------
 * GLFWMetalWindow.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/23 10:03:16
 * ---------------------------------------------------
 */

#define GLFW_EXPOSE_NATIVE_COCOA

#include "Window/GLFW/GLFWMetalWindow.hpp"

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <AppKit/NSWindow.h>
#include <QuartzCore/CAMetalLayer.h>
#include <CoreGraphics/CGGeometry.h>

#include "imguiBackends/imgui_impl_glfw.h"

#include "Graphics/Event.hpp"

namespace gfx
{

#ifdef GFX_IMGUI_ENABLED
void GLFWMetalWindow::imGuiInit()
{
    ImGui_ImplGlfw_InitForOther(m_glfwWindow, true);
}
#endif

CAMetalLayer* GLFWMetalWindow::metalLayer() { @autoreleasepool 
{
    NSWindow* nswindow = glfwGetCocoaWindow(m_glfwWindow);
    return (CAMetalLayer*)nswindow.contentView.layer;
}}

GLFWMetalWindow::GLFWMetalWindow(int w, int h) { @autoreleasepool
{
    ::glfwDefaultWindowHints();
    ::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_glfwWindow = ::glfwCreateWindow(w, h, "", nullptr, nullptr);
    assert(m_glfwWindow);

    NSWindow* nswindow = glfwGetCocoaWindow(m_glfwWindow);
    nswindow.contentView.wantsLayer = YES;
    nswindow.contentView.layer = [CAMetalLayer layer];

    int frameBufferW, frameBufferH;
    ::glfwGetFramebufferSize(m_glfwWindow, &frameBufferW, &frameBufferH);
    ((CAMetalLayer*)nswindow.contentView.layer).drawableSize = CGSizeMake(frameBufferW, frameBufferH);

    setupGLFWcallback();

    addEventCallBack([this](Event& event) {
        event.dispatch<WindowResizeEvent>([this](WindowResizeEvent& event) {
            int frameBufferW, frameBufferH;
            ::glfwGetFramebufferSize(m_glfwWindow, &frameBufferW, &frameBufferH);
            NSWindow* nswindow = glfwGetCocoaWindow(m_glfwWindow);
            ((CAMetalLayer*)nswindow.contentView.layer).drawableSize = CGSizeMake(frameBufferW, frameBufferH);
        });
    }, this);
}}

}