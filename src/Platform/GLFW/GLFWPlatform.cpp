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

#ifdef USING_METAL
    #include "Window/GLFW/GLFWMetalWindow.hpp"
#endif
#ifdef USING_OPENGL
    #include "Window/GLFW/GLFWOpenGLWindow.hpp"
#endif

using utils::SharedPtr;
using utils::UniquePtr;

namespace gfx
{

UniquePtr<Platform> Platform::s_shared;

void Platform::init()
{
    s_shared = UniquePtr<Platform>(new GLFWPlatform());
}

void Platform::terminate()
{
    s_shared.clear();
}

#ifdef USING_METAL
utils::SharedPtr<Window> GLFWPlatform::newMetalWindow(int w, int h) const
{
    SharedPtr<Window> newWindow = SharedPtr<Window>(new GLFWMetalWindow(w, h));
    newWindow->setEventCallBack([this](Event& event){
        if (m_eventCallBack)
            m_eventCallBack(event);
    });
    return newWindow;
}
#endif

#ifdef USING_OPENGL
utils::SharedPtr<Window> GLFWPlatform::newOpenGLWindow(int w, int h) const
{
    SharedPtr<Window> newWindow = SharedPtr<Window>(new GLFWOpenGLWindow(w, h));
    newWindow->setEventCallBack([this](Event& event){
        if (m_eventCallBack)
            m_eventCallBack(event);
    });
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
    if(::glfwInit() != GLFW_TRUE)
        throw GLFWInitError();
}

}
