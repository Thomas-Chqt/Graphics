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
#include "Window/GLFW/GLFWWindow.hpp"
#include "Window/OpenGLWindow.hpp"
#include <GLFW/glfw3.h>
#include "Platform/GLFW/GLFWPlatform.hpp"

namespace gfx
{

class GLFWOpenGLWindow final : public GLFWWindow, public OpenGLWindow
{
private:
    friend utils::SharedPtr<Window> GLFWPlatform::newOpenGLWindow(int w, int h) const;

public:
    GLFWOpenGLWindow(const GLFWOpenGLWindow&) = delete;
    GLFWOpenGLWindow(GLFWOpenGLWindow&&)      = delete;

    #ifdef GFX_IMGUI_ENABLED
        void imGuiInit() override;
    #endif

    inline void makeContextCurrent() override { ::glfwMakeContextCurrent(m_glfwWindow); }
    inline void swapBuffer() override { ::glfwSwapBuffers(m_glfwWindow); }

    ~GLFWOpenGLWindow() override = default;

protected:
    GLFWOpenGLWindow(int w, int h);

public:
    GLFWOpenGLWindow& operator = (const GLFWOpenGLWindow&) = delete;
    GLFWOpenGLWindow& operator = (GLFWOpenGLWindow&&)      = delete;
};

}

#endif // GLFWOPENGLWINDOW_HPP