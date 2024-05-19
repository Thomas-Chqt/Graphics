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
#include "Window/GLFW/GLFWWindow.hpp"
#include "UtilsCPP/Func.hpp"

using utils::SharedPtr;
using gfx::GLFWWindow;
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

SharedPtr<Window> GLFWPlatform::newWindow(int w, int h)
{
    return SharedPtr<Window>(new GLFWWindow(w, h, Func<void(Event&)>(*this, &GLFWPlatform::eventCallBack)));
}

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
