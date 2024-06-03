/*
 * ---------------------------------------------------
 * GLFWWindow.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/29 20:08:44
 * ---------------------------------------------------
 */

#include "Window/GLFW/GLFWWindow.hpp"
#include "GLFW/glfw3.h"

#ifdef IMGUI_ENABLED
    bool ImGui_ImplGlfw_InitForOther(GLFWwindow* window, bool install_callbacks);
    void ImGui_ImplGlfw_Shutdown();
    void ImGui_ImplGlfw_NewFrame();
#endif

namespace gfx
{

void GLFWWindow::setEventCallBack(const utils::Func<void(Event&)>& cb)
{
    m_callback = cb;
}

#ifdef IMGUI_ENABLED
void GLFWWindow::imGuiShutdown()
{
    ImGui_ImplGlfw_Shutdown();
}

void GLFWWindow::imGuiNewFrame()
{
    ImGui_ImplGlfw_NewFrame();
}
#endif

GLFWWindow::~GLFWWindow()
{
    ::glfwDestroyWindow(m_glfwWindow);
}

void GLFWWindow::setupGLFWcallback()
{
    glfwSetWindowUserPointer(m_glfwWindow, this);

    ::glfwSetKeyCallback(m_glfwWindow, [](::GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
    {
        utils::Func<void(Event&)> callback = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow))->m_callback;
        Window& gfxWindow                  = *static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));

        if (action == GLFW_PRESS)
        {
            KeyDownEvent keyDownEvent(gfxWindow, key, false);
            callback(keyDownEvent);
            return;
        }
        if (action == GLFW_REPEAT)
        {
            KeyDownEvent keyDownEvent(gfxWindow, key, true);
            callback(keyDownEvent);
            return;
        }
        if (action == GLFW_RELEASE)
        {
            KeyUpEvent keyUpEvent(gfxWindow, key);
            callback(keyUpEvent);
            return;
        }
    });

    ::glfwSetMouseButtonCallback(m_glfwWindow, [](::GLFWwindow* glfwWindow, int button, int action, int mods)
    {
        utils::Func<void(Event&)> callback = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow))->m_callback;
        Window& gfxWindow                  = *static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));

        double x, y;
        ::glfwGetCursorPos(glfwWindow, &x, &y);

        if (action == GLFW_PRESS)
        {
            MouseDownEvent mouseDownEvent(gfxWindow, (int)x, (int)y, button);
            callback(mouseDownEvent);
            return;
        }
        if (action == GLFW_RELEASE)
        {
            MouseUpEvent mouseUpEvent(gfxWindow, (int)x, (int)y, button);
            callback(mouseUpEvent);
            return;
        }   
    });

    ::glfwSetScrollCallback(m_glfwWindow, [](::GLFWwindow* glfwWindow, double xoffset, double yoffset)
    {
        utils::Func<void(Event&)> callback = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow))->m_callback;
        Window& gfxWindow                  = *static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));

        double x, y;
        ::glfwGetCursorPos(glfwWindow, &x, &y);

        ScrollEvent scrollEvent(gfxWindow, (int)x, (int)y, xoffset, yoffset);
        callback(scrollEvent);
        return;
    });

    ::glfwSetCursorPosCallback(m_glfwWindow, [](::GLFWwindow* glfwWindow, double x, double y)
    {
        utils::Func<void(Event&)> callback = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow))->m_callback;
        Window& gfxWindow                  = *static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));

        MouseMoveEvent mouseMoveEvent(gfxWindow, (int)x, (int)y);
        callback(mouseMoveEvent);
        return;
    });

    ::glfwSetWindowSizeCallback(m_glfwWindow, [](::GLFWwindow* glfwWindow, int width, int height)
    {
        utils::Func<void(Event&)> callback = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow))->m_callback;
        Window& gfxWindow                  = *static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));

        WindowResizeEvent windowResizeEvent(gfxWindow, width, height);
        callback(windowResizeEvent);
    });

    ::glfwSetWindowCloseCallback(m_glfwWindow, [](::GLFWwindow* glfwWindow)
    {
        utils::Func<void(Event&)> callback = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow))->m_callback;
        Window& gfxWindow                  = *static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));

        ::glfwSetWindowShouldClose(glfwWindow, GLFW_FALSE);
        
        WindowRequestCloseEvent windowRequestCloseEvent(gfxWindow);
        callback(windowRequestCloseEvent);
    });
}

}