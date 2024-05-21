/*
 * ---------------------------------------------------
 * GLFWWindow.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/01/30 16:19:39
 * ---------------------------------------------------
 */

#include "Window/GLFW/GLFWWindow.hpp"

#include <GLFW/glfw3.h>
#include <cassert>

using gfx::GLFWWindow;

namespace gfx
{

GLFWWindow::GLFWWindow(int w, int h, const utils::Func<void(Event&)>& defaultCallback) : m_nextEventCallback(defaultCallback)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    m_glfwWindow = ::glfwCreateWindow(w, h, "", nullptr, nullptr);
    assert(m_glfwWindow);

    glfwSetWindowUserPointer(m_glfwWindow, this);

    ::glfwSetKeyCallback(m_glfwWindow, [](::GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        GLFWWindow& _this = *(GLFWWindow*)glfwGetWindowUserPointer(window);

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

        GLFWWindow& _this = *(GLFWWindow*)glfwGetWindowUserPointer(window);

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
        GLFWWindow& _this = *(GLFWWindow*)glfwGetWindowUserPointer(window);

        MouseMoveEvent mouseMoveEvent(_this, (int)x, (int)y);
        _this.eventCallBack(mouseMoveEvent);
        return;
    });

    ::glfwSetWindowSizeCallback(m_glfwWindow, [](::GLFWwindow* window, int width, int height)
    {
        GLFWWindow& _this = *(GLFWWindow*)glfwGetWindowUserPointer(window);

        WindowResizeEvent windowResizeEvent(_this, width, height);
        _this.eventCallBack(windowResizeEvent);
    });

    ::glfwSetWindowCloseCallback(m_glfwWindow, [](::GLFWwindow* window)
    {
        glfwSetWindowShouldClose(window, GLFW_FALSE);
        GLFWWindow& _this = *(GLFWWindow*)glfwGetWindowUserPointer(window);
        
        WindowRequestCloseEvent windowRequestCloseEvent(_this);
        _this.eventCallBack(windowRequestCloseEvent);
    });
}

GLFWWindow::~GLFWWindow()
{
    ::glfwDestroyWindow(m_glfwWindow);
}

}