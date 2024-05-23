/*
 * ---------------------------------------------------
 * GLFWPlatform.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/01/20 13:08:22
 * ---------------------------------------------------
 */

#include "Platform/GLFW/GLFWPlatform.hpp"

#include <GLFW/glfw3.h>
#include <cassert>

#include "Logger/Logger.hpp"
#ifdef USING_METAL
    #include "Window/GLFW/GLFWMetalWindow.hpp"
#endif
#include "UtilsCPP/Func.hpp"
#include "Window/GLFW/GLFWOpenGLWindow.hpp"

using utils::SharedPtr;
using utils::UniquePtr;
using utils::Func;

namespace gfx
{

UniquePtr<Platform> Platform::s_shared;

void Platform::init()
{
    s_shared = UniquePtr<Platform>(new GLFWPlatform());
    logDebug << "GLFWPlatform initialized" << std::endl;
}

void Platform::terminate()
{
    s_shared.clear();
    logDebug << "GLFWPlatform terminated" << std::endl;
}

#ifdef USING_METAL
utils::SharedPtr<Window> GLFWPlatform::newMetalWindow(int w, int h)
{
    return SharedPtr<Window>(new GLFWMetalWindow(w, h, Func<void(Event&)>(*this, &GLFWPlatform::eventCallBack)));
}
#endif

#ifdef USING_OPENGL
utils::SharedPtr<Window> GLFWPlatform::newOpenGLWindow(int w, int h)
{
    return SharedPtr<Window>(new GLFWOpenGLWindow(w, h, Func<void(Event&)>(*this, &GLFWPlatform::eventCallBack)));
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
    int ret = ::glfwInit();
    assert(ret == GLFW_TRUE);
}

void GLFWPlatform::eventCallBack(Event& e)
{
    if (m_nextEventCallback)
        m_nextEventCallback(e);
}

}
