/*
 * ---------------------------------------------------
 * GLFWWindow.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/01/30 16:18:27
 * ---------------------------------------------------
 */

#ifndef GLFWWINDOW_HPP
# define GLFWWINDOW_HPP

#include "Graphics/Window.hpp"

#include "Platform/GLFW/GLFWPlatform.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include <GLFW/glfw3.h>

namespace gfx
{

class GLFWWindow : public Window
{
private:
    friend utils::SharedPtr<Window> GLFWPlatform::newWindow(int, int);

public:
    GLFWWindow()                  = delete;
    GLFWWindow(const GLFWWindow&) = delete;
    GLFWWindow(GLFWWindow&&)      = delete;

    inline void setEventCallBack(const utils::Func<void(Event&)>& cb) override { m_nextEventCallback = cb; }

#ifdef USING_OPENGL
    inline void useOpenGL() override { makeOpenGlContextCurrent(); };
    inline void makeOpenGlContextCurrent() override { ::glfwMakeContextCurrent(m_glfwWindow); };
    inline void openGLSwapBuffer() override { ::glfwSwapBuffers(m_glfwWindow); };
#endif

    ~GLFWWindow() override;

private:
    GLFWWindow(int w, int h, const utils::Func<void(Event&)>& defaultCallback);

    inline void eventCallBack(Event& e) { m_nextEventCallback(e); }

    ::GLFWwindow* m_glfwWindow = nullptr;
    utils::Func<void(Event&)> m_nextEventCallback;

public:
    GLFWWindow operator = (const GLFWWindow&) = delete;
    GLFWWindow operator = (GLFWWindow&&)      = delete;
};

}

#endif // GLFWWINDOW_HPP