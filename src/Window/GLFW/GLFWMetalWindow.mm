/*
 * ---------------------------------------------------
 * GLFWMetalWindow.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/23 10:03:16
 * ---------------------------------------------------
 */

#define GLFW_EXPOSE_NATIVE_COCOA

#include "GraphicAPI/Metal/MetalTexture.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Texture.hpp"
#include "Window/GLFW/GLFWMetalWindow.hpp"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <AppKit/NSWindow.h>
#include <QuartzCore/CAMetalLayer.h>
#include <CoreGraphics/CGGeometry.h>
#include "Graphics/Event.hpp"
#include "GraphicAPI/Metal/MetalGraphicAPI.hpp"

#ifdef GFX_BUILD_IMGUI
    #include <imgui_impl_glfw.h>
#endif

namespace gfx
{

GLFWMetalWindow::GLFWMetalWindow(int w, int h) { @autoreleasepool
{
    ::glfwDefaultWindowHints();
    ::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_glfwWindow = ::glfwCreateWindow(w, h, "", nullptr, nullptr);
    assert(m_glfwWindow);

    NSWindow* nswindow = glfwGetCocoaWindow(m_glfwWindow);
    nswindow.contentView.wantsLayer = YES;
    nswindow.contentView.layer = [CAMetalLayer layer];

    int frameBufferW = 0;
    int frameBufferH = 0;
    ::glfwGetFramebufferSize(m_glfwWindow, &frameBufferW, &frameBufferH);
    ((CAMetalLayer*)nswindow.contentView.layer).drawableSize = CGSizeMake(frameBufferW, frameBufferH);

    setupGLFWcallback();

    GLFWMetalWindow::addEventCallBack([this](Event& event) {
        event.dispatch<WindowResizeEvent>([this](WindowResizeEvent&) {
            int frameBufferW = 0;
            int frameBufferH = 0;
            ::glfwGetFramebufferSize(m_glfwWindow, &frameBufferW, &frameBufferH);
            NSWindow* nswindow = glfwGetCocoaWindow(m_glfwWindow);
            ((CAMetalLayer*)nswindow.contentView.layer).drawableSize = CGSizeMake(frameBufferW, frameBufferH);
            recreateDepthTexture(frameBufferW, frameBufferH);
        });
    }, this);
}}

#ifdef GFX_BUILD_IMGUI
void GLFWMetalWindow::imGuiInit()
{
    ImGui_ImplGlfw_InitForOther(m_glfwWindow, true);
}
#endif

void GLFWMetalWindow::setGraphicAPI(MetalGraphicAPI* api) { @autoreleasepool
{
    if (m_graphicAPI != api)
    {
        m_graphicAPI = api;
        NSWindow* nswindow = glfwGetCocoaWindow(m_glfwWindow);
        static_cast<CAMetalLayer*>(nswindow.contentView.layer).device = m_graphicAPI->device();
        int frameBufferW = 0;
        int frameBufferH = 0;
        ::glfwGetFramebufferSize(m_glfwWindow, &frameBufferW, &frameBufferH);
        recreateDepthTexture(frameBufferW, frameBufferH);
    }
}}

void GLFWMetalWindow::makeCurrentDrawables() { @autoreleasepool
{
    NSWindow* nswindow = glfwGetCocoaWindow(m_glfwWindow);
    m_currentDrawable = [static_cast<CAMetalLayer*>(nswindow.contentView.layer) nextDrawable];
    [m_currentDrawable retain];
}}

void GLFWMetalWindow::clearCurrentDrawables() { @autoreleasepool
{
    [m_currentDrawable release];
    m_currentDrawable = nullptr;
}}

void GLFWMetalWindow::recreateDepthTexture(utils::uint32 w, utils::uint32 h)
{
    Texture::Descriptor descriptor;
    descriptor.width = w;
    descriptor.height = h;
    descriptor.pixelFormat = PixelFormat::Depth32;
    descriptor.storageMode = StorageMode::Private;
    descriptor.usage = Texture::Usage::RenderTarget;
    m_currentDepthTexture = MetalTexture(m_graphicAPI->device(), descriptor);
}

}
