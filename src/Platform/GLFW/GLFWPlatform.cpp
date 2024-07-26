/*
 * ---------------------------------------------------
 * GLFWPlatform.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/01/20 13:08:22
 * ---------------------------------------------------
 */

#include "Platform/GLFW/GLFWPlatform.hpp"
#include "Graphics/Error.hpp"
#include "UtilsCPP/String.hpp"

#include <GLFW/glfw3.h>

#ifdef GFX_BUILD_METAL
    #include "Window/GLFW/GLFWMetalWindow.hpp"
#endif
#ifdef GFX_BUILD_OPENGL
    #include "Window/GLFW/GLFWOpenGLWindow.hpp"
#endif

using utils::SharedPtr;
using utils::UniquePtr;

namespace gfx
{

int GLFWError::s_lastErrorCode = 0;
utils::String GLFWError::s_lastErrorDesc = "No error";

UniquePtr<Platform> Platform::s_shared;

void Platform::init()
{
    s_shared = UniquePtr<Platform>(new GLFWPlatform());
}

void Platform::terminate()
{
    s_shared.clear();
}

#ifdef GFX_BUILD_METAL
utils::SharedPtr<Window> GLFWPlatform::newMetalWindow(int w, int h) const
{
    SharedPtr<Window> newWindow = SharedPtr<Window>(new GLFWMetalWindow(w, h));
    newWindow->addEventCallBack([this](Event& event) {
        for (auto& callbacks : m_eventCallbacks)
        {
            for (auto& callback : callbacks.val)
                callback(event);
        }
    }, (void*)this);
    return newWindow;
}
#endif

void GLFWPlatform::addEventCallBack(const utils::Func<void(Event&)>& cb, void* id)
{
    utils::Dictionary<void*, utils::Array<utils::Func<void(Event&)>>>::Iterator it = m_eventCallbacks.find(id);
    if (it == m_eventCallbacks.end())
        m_eventCallbacks.insert(id, utils::Array<utils::Func<void(Event&)>>({cb}));
    else
        it->val.append(cb);
}

#ifdef GFX_BUILD_OPENGL
utils::SharedPtr<Window> GLFWPlatform::newOpenGLWindow(int w, int h) const
{
    SharedPtr<Window> newWindow = SharedPtr<Window>(new GLFWOpenGLWindow(w, h));
    newWindow->addEventCallBack([this](Event& event) {
        for (const auto& callbacks : m_eventCallbacks)
        {
            for (const auto& callback : callbacks.val)
                callback(event);
        }
    }, (void*)this);
    return newWindow;
}
#endif

void GLFWPlatform::pollEvents()
{
    ::glfwPollEvents();
}

GLFWPlatform::~GLFWPlatform()
{
    ::glfwTerminate();
}

GLFWPlatform::GLFWPlatform()
{
    ::glfwSetErrorCallback([](int code, const char* desc){
        GLFWError::s_lastErrorCode = code;
        GLFWError::s_lastErrorDesc = utils::String(desc);
    });

    if(::glfwInit() != GLFW_TRUE)
        throw GLFWInitError();
}

}
