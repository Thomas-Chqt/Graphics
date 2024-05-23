/*
 * ---------------------------------------------------
 * GLFWOpenGLWindow.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/23 10:22:27
 * ---------------------------------------------------
 */

#ifndef GLFWOPENGLWINDOW_HPP
# define GLFWOPENGLWINDOW_HPP

#include "Graphics/Event.hpp"
#include "UtilsCPP/Func.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Window/OpenGLWindow.hpp"
#include "Platform/GLFW/GLFWPlatform.hpp"
#include <GLFW/glfw3.h>

namespace gfx
{

class GLFWOpenGLWindow : public OpenGLWindow
{
private:
    friend utils::SharedPtr<Window> GLFWPlatform::newOpenGLWindow(int w, int h);

public:
    GLFWOpenGLWindow(const GLFWOpenGLWindow&) = delete;
    GLFWOpenGLWindow(GLFWOpenGLWindow&&)      = delete;

    inline void setEventCallBack(const utils::Func<void(Event&)>& cb) override { m_nextEventCallback = cb; }

#ifdef IMGUI_ENABLED
    void imGuiInit() override;
    void imGuiShutdown() override;
    void imGuiNewFrame() override;
#endif

    inline void makeContextCurrent() override { ::glfwMakeContextCurrent(m_glfwWindow); }
    inline void swapBuffer() override { ::glfwSwapBuffers(m_glfwWindow); }

    ~GLFWOpenGLWindow() override;

protected:
    GLFWOpenGLWindow(int w, int h, const utils::Func<void(Event&)>& defaultCallback);

    inline void eventCallBack(Event& e) { m_nextEventCallback(e); }

    ::GLFWwindow* m_glfwWindow = nullptr;
    utils::Func<void(Event&)> m_nextEventCallback;

public:
    GLFWOpenGLWindow& operator = (const GLFWOpenGLWindow&) = delete;
    GLFWOpenGLWindow& operator = (GLFWOpenGLWindow&&)      = delete;
};

}

#endif // GLFWOPENGLWINDOW_HPP