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

#include "Graphics/Event.hpp"
#include "Platform/GLFW/GLFWPlatform.hpp"
#include "UtilsCPP/Func.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Window/MetalWindow.hpp"
#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#ifdef __OBJC__
    #import <QuartzCore/CAMetalLayer.h>
    #import <AppKit/AppKit.h>
#else
    class CAMetalLayer;
    class NSWindow;
#endif // OBJCPP

namespace gfx
{

class GLFWMetalWindow : public MetalWindow
{
private:
    friend utils::SharedPtr<Window> GLFWPlatform::newMetalWindow(int w, int h);

public:
    GLFWMetalWindow(const GLFWMetalWindow&) = delete;
    GLFWMetalWindow(GLFWMetalWindow&&)      = delete;

    inline void setEventCallBack(const utils::Func<void(Event&)>& cb) override { m_nextEventCallback = cb; }

#ifdef IMGUI_ENABLED
    void imGuiInit() override;
    void imGuiShutdown() override;
    void imGuiNewFrame() override;
#endif

    CAMetalLayer* metalLayer() override;

    ~GLFWMetalWindow() override;

protected:
    GLFWMetalWindow(int w, int h, const utils::Func<void(Event&)>& defaultCallback);

    inline void eventCallBack(Event& e) { m_nextEventCallback(e); }

    ::GLFWwindow* m_glfwWindow = nullptr;
    utils::Func<void(Event&)> m_nextEventCallback;
    NSWindow* m_nswindow = nullptr;

public:
    GLFWMetalWindow& operator = (const GLFWMetalWindow&) = delete;
    GLFWMetalWindow& operator = (GLFWMetalWindow&&)      = delete;
};

}

#endif // GLFWMETALWINDOW_HPP