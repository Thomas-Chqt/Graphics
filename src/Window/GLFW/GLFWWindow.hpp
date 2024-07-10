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
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Func.hpp"
#include "Window/Window_internal.hpp"
#include "UtilsCPP/Dictionary.hpp"

namespace gfx
{

class GLFWWindow : virtual public Window_internal
{
public:
    GLFWWindow()                  = default;
    GLFWWindow(const GLFWWindow&) = delete;
    GLFWWindow(GLFWWindow&&)      = delete;

    inline void addEventCallBack(const utils::Func<void(Event&)>& cb, void* id = (void*)0) override { m_eventCallbacks.get(id).append(cb); }
    inline void clearCallbacks(void* id = (void*)0) override { m_eventCallbacks.remove(id); }

    inline bool isKeyPress(int key) override { return ::glfwGetKey(m_glfwWindow, key) == GLFW_PRESS; }
    inline bool isMousePress(int button) override { return ::glfwGetMouseButton(m_glfwWindow, button) == GLFW_PRESS; }

    inline void setCursorPos(int x, int y) override { ::glfwSetCursorPos(m_glfwWindow, (double)x, (double)y); }
    inline void setCursorVisibility(bool val) override { ::glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, val ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN); }

    #ifdef GFX_BUILD_IMGUI
        void imGuiShutdown() override;
        void imGuiNewFrame() override;
    #endif

    void getWindowSize(utils::uint32* width, utils::uint32* height) const override;
    void getFrameBufferSize(utils::uint32* width, utils::uint32* height) const override;
    void getContentScale(float* xscale, float* yscale) const override;

    ~GLFWWindow();

protected:
    void setupGLFWcallback();

    ::GLFWwindow* m_glfwWindow = nullptr;
    utils::Dictionary<void*, utils::Array<utils::Func<void(Event&)>>> m_eventCallbacks;

public:
    GLFWWindow& operator = (const GLFWWindow&) = delete;
    GLFWWindow& operator = (GLFWWindow&&)      = delete;
};

}

#endif // GLFWWINDOW_HPP