/*
 * ---------------------------------------------------
 * GLFWMetalWindow.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/23 10:03:16
 * ---------------------------------------------------
 */

#include "Window/GLFW/GLFWMetalWindow.hpp"
#include "Logger/Logger.hpp"
#include <CoreGraphics/CGGeometry.h>
#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#ifdef IMGUI_ENABLED
    bool ImGui_ImplGlfw_InitForOther(GLFWwindow* window, bool install_callbacks);
    void ImGui_ImplGlfw_Shutdown();
    void ImGui_ImplGlfw_NewFrame();
#endif

namespace gfx
{

#ifdef IMGUI_ENABLED
void GLFWMetalWindow::imGuiInit()
{
    ImGui_ImplGlfw_InitForOther(m_glfwWindow, true);
}

void GLFWMetalWindow::imGuiShutdown()
{
    ImGui_ImplGlfw_Shutdown();
}

void GLFWMetalWindow::imGuiNewFrame()
{
    ImGui_ImplGlfw_NewFrame();
}
#endif

CAMetalLayer* GLFWMetalWindow::metalLayer() { @autoreleasepool 
{
    return (CAMetalLayer*)m_nswindow.contentView.layer;
}}

GLFWMetalWindow::~GLFWMetalWindow()
{
    ::glfwDestroyWindow(m_glfwWindow);
    logDebug << "GLFWMetalWindow (" << this << ") destructed" << std::endl;
}

GLFWMetalWindow::GLFWMetalWindow(int w, int h, const utils::Func<void(Event&)>& defaultCallback) 
    : m_nextEventCallback(defaultCallback) { @autoreleasepool
{
    ::glfwDefaultWindowHints();
    
    ::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_glfwWindow = ::glfwCreateWindow(w, h, "", nullptr, nullptr);
    assert(m_glfwWindow);

    m_nswindow = glfwGetCocoaWindow(m_glfwWindow);
    m_nswindow.contentView.wantsLayer = YES;

    m_nswindow.contentView.layer = [CAMetalLayer layer];

    int frameBufferW, frameBufferH;
    ::glfwGetFramebufferSize(m_glfwWindow, &frameBufferW, &frameBufferH);
    ((CAMetalLayer*)m_nswindow.contentView.layer).drawableSize = CGSizeMake(frameBufferW, frameBufferH);

    glfwSetWindowUserPointer(m_glfwWindow, this);

    ::glfwSetKeyCallback(m_glfwWindow, [](::GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        GLFWMetalWindow& _this = *(GLFWMetalWindow*)glfwGetWindowUserPointer(window);

        if (action == GLFW_PRESS)
        {
            KeyDownEvent keyDownEvent(_this, key, false);
            _this.eventCallBack(keyDownEvent);
            return;
        }
        if (action == GLFW_REPEAT)
        {
            KeyDownEvent keyDownEvent(_this, key, true);
            _this.eventCallBack(keyDownEvent);
            return;
        }
        if (action == GLFW_RELEASE)
        {
            KeyUpEvent keyUpEvent(_this, key);
            _this.eventCallBack(keyUpEvent);
            return;
        }
    });

    ::glfwSetMouseButtonCallback(m_glfwWindow, [](::GLFWwindow* window, int button, int action, int mods)
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);

        GLFWMetalWindow& _this = *(GLFWMetalWindow*)glfwGetWindowUserPointer(window);

        if (action == GLFW_PRESS)
        {
            MouseDownEvent mouseDownEvent(_this, button, (int)x, (int)y);
            _this.eventCallBack(mouseDownEvent);
            return;
        }
        if (action == GLFW_RELEASE)
        {
            MouseUpEvent mouseUpEvent(_this, button, (int)x, (int)y);
            _this.eventCallBack(mouseUpEvent);
            return;
        }   
    });

    ::glfwSetCursorPosCallback(m_glfwWindow, [](::GLFWwindow* window, double x, double y)
    {
        GLFWMetalWindow& _this = *(GLFWMetalWindow*)glfwGetWindowUserPointer(window);

        MouseMoveEvent mouseMoveEvent(_this, (int)x, (int)y);
        _this.eventCallBack(mouseMoveEvent);
        return;
    });

    ::glfwSetWindowSizeCallback(m_glfwWindow, [](::GLFWwindow* window, int width, int height) { @autoreleasepool
    {
        GLFWMetalWindow& _this = *(GLFWMetalWindow*)glfwGetWindowUserPointer(window);

        int frameBufferW, frameBufferH;
        ::glfwGetFramebufferSize(window, &frameBufferW, &frameBufferH);
        ((CAMetalLayer*)_this.m_nswindow.contentView.layer).drawableSize = CGSizeMake(frameBufferW, frameBufferH);

        WindowResizeEvent windowResizeEvent(_this, width, height);
        _this.eventCallBack(windowResizeEvent);
    }});

    ::glfwSetWindowCloseCallback(m_glfwWindow, [](::GLFWwindow* window)
    {
        glfwSetWindowShouldClose(window, GLFW_FALSE);
        GLFWMetalWindow& _this = *(GLFWMetalWindow*)glfwGetWindowUserPointer(window);
        
        WindowRequestCloseEvent windowRequestCloseEvent(_this);
        _this.eventCallBack(windowRequestCloseEvent);
    });

    logDebug << "GLFWMetalWindow (" << this << ") created" << std::endl;
}}

}