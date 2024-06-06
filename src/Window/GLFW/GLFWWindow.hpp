/*
 * ---------------------------------------------------
 * GLFWWindow.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/29 20:01:10
 * ---------------------------------------------------
 */

#ifndef GLFWWINDOW_HPP
# define GLFWWINDOW_HPP

#include <GLFW/glfw3.h>
#include "Graphics/Event.hpp"
#include "UtilsCPP/Func.hpp"
#include "Window/Window_internal.hpp"

namespace gfx
{

class GLFWWindow : virtual public Window_internal
{
public:
    GLFWWindow()                  = default;
    GLFWWindow(const GLFWWindow&) = delete;
    GLFWWindow(GLFWWindow&&)      = delete;

    virtual void setEventCallBack(const utils::Func<void(Event&)>& cb);

    inline void setCursorPos(int x, int y) { ::glfwSetCursorPos(m_glfwWindow, (double)x, (double)y); }
    inline void setCursorVisibility(bool val) { ::glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, val ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN); }

    #ifdef GFX_IMGUI_ENABLED
        void imGuiShutdown();
        void imGuiNewFrame();
    #endif
    
    ~GLFWWindow();

protected:
    void setupGLFWcallback();

    ::GLFWwindow* m_glfwWindow = nullptr;
    utils::Func<void(Event&)> m_callback;

public:
    GLFWWindow& operator = (const GLFWWindow&) = delete;
    GLFWWindow& operator = (GLFWWindow&&)      = delete;
};

}

#endif // GLFWWINDOW_HPP