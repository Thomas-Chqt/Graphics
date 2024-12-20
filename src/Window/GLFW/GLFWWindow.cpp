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
#include "Graphics/Event.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Types.hpp"
#include <cassert>
#include <filesystem>

#ifdef GFX_BUILD_IMGUI
    #include <imgui_impl_glfw.h>
#endif

namespace fs = std::filesystem;

namespace gfx
{

utils::uint32 GLFWWindow::width()
{
    int w, h;
    ::glfwGetFramebufferSize(m_glfwWindow, &w, &h);
    assert(w >= 0);
    return static_cast<utils::uint32>(w);
}

utils::uint32 GLFWWindow::height()
{
    int w, h;
    ::glfwGetFramebufferSize(m_glfwWindow, &w, &h);
    assert(h >= 0);
    return static_cast<utils::uint32>(h);
}

void GLFWWindow::addEventCallBack(const utils::Func<void(Event&)>& cb, void* id)
{
    utils::Dictionary<void*, utils::Array<utils::Func<void(Event&)>>>::Iterator it = m_eventCallbacks.find(id);
    if (it == m_eventCallbacks.end())
        m_eventCallbacks.insert(id, utils::Array<utils::Func<void(Event&)>>({cb}));
    else
        it->val.append(cb);
}

#ifdef GFX_BUILD_IMGUI
void GLFWWindow::imGuiShutdown()
{
    ImGui_ImplGlfw_Shutdown();
}

void GLFWWindow::imGuiNewFrame()
{
    ImGui_ImplGlfw_NewFrame();
}
#endif

void GLFWWindow::getWindowSize(utils::uint32* width, utils::uint32* height) const
{
    ::glfwGetWindowSize(m_glfwWindow, (int*)width, (int*)height);
}

void GLFWWindow::getFrameBufferSize(utils::uint32* width, utils::uint32* height) const
{
    ::glfwGetFramebufferSize(m_glfwWindow, (int*)width, (int*)height);
}

void GLFWWindow::getContentScale(float* xscale, float* yscale) const
{
    ::glfwGetWindowContentScale(m_glfwWindow, xscale, yscale);
}

void GLFWWindow::getFrameBufferScaleFactor(float* xscale, float* yscale) const
{
    int width, height;
    ::glfwGetWindowSize(m_glfwWindow, &width, &height);

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(m_glfwWindow, &fbWidth, &fbHeight);

    *xscale = (float)fbWidth / (float)width;
    *yscale = (float)fbHeight / (float)height;
}

bool GLFWWindow::popDroppedFile(std::filesystem::path& dst)
{
    if (m_droppedFilePool.isEmpty())
        return false;
    dst = m_droppedFilePool.pop(m_droppedFilePool.begin());
    return true;
}

void GLFWWindow::setClipboardString(const utils::String& str) const
{
    ::glfwSetClipboardString(m_glfwWindow, str);
}

utils::String GLFWWindow::getClipboardString() const
{
    return ::glfwGetClipboardString(m_glfwWindow);
}

GLFWWindow::~GLFWWindow()
{
    ::glfwDestroyWindow(m_glfwWindow);
}

void GLFWWindow::setupGLFWcallback()
{
    using CallbackDict = utils::Dictionary<void*, utils::Array<utils::Func<void(Event&)>>>;

    glfwSetWindowUserPointer(m_glfwWindow, this);

    ::glfwSetKeyCallback(m_glfwWindow, [](::GLFWwindow* glfwWindow, int key, int, int action, int)
    {
        CallbackDict callbackDict = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow))->m_eventCallbacks;
        Window& gfxWindow = *static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));

        if (action == GLFW_PRESS)
        {
            KeyDownEvent keyDownEvent(gfxWindow, key, false);
            for (auto& callbacks : callbackDict)
            {
                for (auto& callback : callbacks.val)
                    callback(keyDownEvent);
            }
        }
        else if (action == GLFW_REPEAT)
        {
            KeyDownEvent keyDownEvent(gfxWindow, key, true);
            for (auto& callbacks : callbackDict)
            {
                for (auto& callback : callbacks.val)
                    callback(keyDownEvent);
            }
        }
        else if (action == GLFW_RELEASE)
        {
            KeyUpEvent keyUpEvent(gfxWindow, key);
            for (auto& callbacks : callbackDict)
            {
                for (auto& callback : callbacks.val)
                    callback(keyUpEvent);
            }
        }
    });

    ::glfwSetMouseButtonCallback(m_glfwWindow, [](::GLFWwindow* glfwWindow, int button, int action, int)
    {
        CallbackDict callbackDict = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow))->m_eventCallbacks;
        Window& gfxWindow = *static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));

        double x = 0;
        double y = 0;
        ::glfwGetCursorPos(glfwWindow, &x, &y);

        if (action == GLFW_PRESS)
        {
            MouseDownEvent mouseDownEvent(gfxWindow, (int)x, (int)y, button);
            for (auto& callbacks : callbackDict)
            {
                for (auto& callback : callbacks.val)
                    callback(mouseDownEvent);
            }
        }
        else if (action == GLFW_RELEASE)
        {
            MouseUpEvent mouseUpEvent(gfxWindow, (int)x, (int)y, button);
            for (auto& callbacks : callbackDict)
            {
                for (auto& callback : callbacks.val)
                    callback(mouseUpEvent);
            }
        }   
    });

    ::glfwSetScrollCallback(m_glfwWindow, [](::GLFWwindow* glfwWindow, double xoffset, double yoffset)
    {
        CallbackDict callbackDict = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow))->m_eventCallbacks;
        Window& gfxWindow = *static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));

        double x = 0;
        double y = 0;
        ::glfwGetCursorPos(glfwWindow, &x, &y);

        ScrollEvent scrollEvent(gfxWindow, (int)x, (int)y, xoffset, yoffset);
        for (auto& callbacks : callbackDict)
        {
            for (auto& callback : callbacks.val)
                callback(scrollEvent);
        }
    });

    ::glfwSetCursorPosCallback(m_glfwWindow, [](::GLFWwindow* glfwWindow, double x, double y)
    {
        CallbackDict callbackDict = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow))->m_eventCallbacks;
        Window& gfxWindow = *static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));

        MouseMoveEvent mouseMoveEvent(gfxWindow, (int)x, (int)y);
        for (auto& callbacks : callbackDict)
        {
            for (auto& callback : callbacks.val)
                callback(mouseMoveEvent);
        }
    });

    ::glfwSetWindowSizeCallback(m_glfwWindow, [](::GLFWwindow* glfwWindow, int width, int height)
    {
        CallbackDict callbackDict = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow))->m_eventCallbacks;
        Window& gfxWindow = *static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));

        WindowResizeEvent windowResizeEvent(gfxWindow, width, height);
        for (auto& callbacks : callbackDict)
        {
            for (auto& callback : callbacks.val)
                callback(windowResizeEvent);
        }
    });

    ::glfwSetWindowCloseCallback(m_glfwWindow, [](::GLFWwindow* glfwWindow)
    {
        CallbackDict callbackDict = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow))->m_eventCallbacks;
        Window& gfxWindow = *static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));

        ::glfwSetWindowShouldClose(glfwWindow, GLFW_FALSE);
        
        WindowRequestCloseEvent windowRequestCloseEvent(gfxWindow);
        for (auto& callbacks : callbackDict)
        {
            for (auto& callback : callbacks.val)
                callback(windowRequestCloseEvent);
        }
    });

    ::glfwSetDropCallback(m_glfwWindow, [](::GLFWwindow* glfwWindow, int count, const char** paths){
        GLFWWindow& gfxWindow = *static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));
        for (int i = 0; i < count; i++)
        {
            fs::path path = fs::path(paths[i]);
            assert(path.is_absolute());
            assert(fs::exists(path));
            gfxWindow.m_droppedFilePool.insert(path);
        }
    });
}

}