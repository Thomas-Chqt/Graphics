/*
 * ---------------------------------------------------
 * GLFWMetalWindow.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/23 09:56:51
 * ---------------------------------------------------
 */

#ifndef GLFWMETALWINDOW_HPP
# define GLFWMETALWINDOW_HPP

#include "Window/GLFW/GLFWWindow.hpp"
#include "Window/MetalWindow.hpp"

#include <GLFW/glfw3.h>

#include "Platform/GLFW/GLFWPlatform.hpp"
#include "UtilsCPP/Func.hpp"
#include "Graphics/Event.hpp"
#include "UtilsCPP/SharedPtr.hpp"

#ifdef __OBJC__
    #import <QuartzCore/CAMetalLayer.h>
#else
    class CAMetalLayer;
    class NSWindow;
#endif // OBJCPP

namespace gfx
{

class GLFWMetalWindow final : public GLFWWindow, public MetalWindow
{
private:
    friend utils::SharedPtr<Window> GLFWPlatform::newMetalWindow(int w, int h) const;

public:
    GLFWMetalWindow(const GLFWMetalWindow&) = delete;
    GLFWMetalWindow(GLFWMetalWindow&&)      = delete;

    void setEventCallBack(const utils::Func<void(Event&)>& cb) override;
    
    #ifdef IMGUI_ENABLED
        void imGuiInit() override;
    #endif

    CAMetalLayer* metalLayer() override;

    ~GLFWMetalWindow() override = default;

private:
    GLFWMetalWindow(int w, int h);

public:
    GLFWMetalWindow& operator = (const GLFWMetalWindow&) = delete;
    GLFWMetalWindow& operator = (GLFWMetalWindow&&)      = delete;
};

}

#endif // GLFWMETALWINDOW_HPP