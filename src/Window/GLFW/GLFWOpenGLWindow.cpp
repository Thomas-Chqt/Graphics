/*
 * ---------------------------------------------------
 * GLFWOpenGLWindow.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/23 10:24:41
 * ---------------------------------------------------
 */

#include "Window/GLFW/GLFWOpenGLWindow.hpp"
#include "Logger/Logger.hpp"
#include <GLFW/glfw3.h>
#include <cassert>

#ifdef IMGUI_ENABLED
    bool ImGui_ImplGlfw_InitForOpenGL(GLFWwindow* window, bool install_callbacks);
    void ImGui_ImplGlfw_Shutdown();
    void ImGui_ImplGlfw_NewFrame();
#endif

namespace gfx
{

#ifdef IMGUI_ENABLED
void GLFWOpenGLWindow::imGuiInit()
{
    ImGui_ImplGlfw_InitForOpenGL(m_glfwWindow, true);
}

void GLFWOpenGLWindow::imGuiShutdown()
{
    ImGui_ImplGlfw_Shutdown();
}

void GLFWOpenGLWindow::imGuiNewFrame()
{
    ImGui_ImplGlfw_NewFrame();
}
#endif


GLFWOpenGLWindow::~GLFWOpenGLWindow()
{
    ::glfwDestroyWindow(m_glfwWindow);
    logDebug << "GLFWOpenGLWindow (" << this << ") destructed" << std::endl;
}

GLFWOpenGLWindow::GLFWOpenGLWindow(int w, int h, const utils::Func<void(Event&)>& defaultCallback)
    : m_nextEventCallback(defaultCallback)
{
    ::glfwDefaultWindowHints();

    #ifdef __APPLE__
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    #endif
    
    m_glfwWindow = ::glfwCreateWindow(w, h, "", nullptr, nullptr);
    assert(m_glfwWindow);

    glfwSetWindowUserPointer(m_glfwWindow, this);

    ::glfwSetKeyCallback(m_glfwWindow, [](::GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        GLFWOpenGLWindow& _this = *(GLFWOpenGLWindow*)glfwGetWindowUserPointer(window);

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

        GLFWOpenGLWindow& _this = *(GLFWOpenGLWindow*)glfwGetWindowUserPointer(window);

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
        GLFWOpenGLWindow& _this = *(GLFWOpenGLWindow*)glfwGetWindowUserPointer(window);

        MouseMoveEvent mouseMoveEvent(_this, (int)x, (int)y);
        _this.eventCallBack(mouseMoveEvent);
        return;
    });

    ::glfwSetWindowSizeCallback(m_glfwWindow, [](::GLFWwindow* window, int width, int height)
    {
        GLFWOpenGLWindow& _this = *(GLFWOpenGLWindow*)glfwGetWindowUserPointer(window);

        WindowResizeEvent windowResizeEvent(_this, width, height);
        _this.eventCallBack(windowResizeEvent);
    });

    ::glfwSetWindowCloseCallback(m_glfwWindow, [](::GLFWwindow* window)
    {
        glfwSetWindowShouldClose(window, GLFW_FALSE);
        GLFWOpenGLWindow& _this = *(GLFWOpenGLWindow*)glfwGetWindowUserPointer(window);
        
        WindowRequestCloseEvent windowRequestCloseEvent(_this);
        _this.eventCallBack(windowRequestCloseEvent);
    });

    logDebug << "GLFWOpenGLWindow (" << this << ") created" << std::endl;
}

}