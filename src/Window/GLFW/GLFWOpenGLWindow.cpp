/*
 * ---------------------------------------------------
 * GLFWOpenGLWindow.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/23 10:24:41
 * ---------------------------------------------------
 */

#include "Window/GLFW/GLFWOpenGLWindow.hpp"
#include "Graphics/Error.hpp"
#include <GLFW/glfw3.h>
#include <cassert>
#include <glad/glad.h>

#ifdef GFX_BUILD_IMGUI
#include "imguiBackends/imgui_impl_glfw.h"
#endif

namespace gfx
{

#ifdef GFX_BUILD_IMGUI
void GLFWOpenGLWindow::imGuiInit()
{
    ImGui_ImplGlfw_InitForOpenGL(m_glfwWindow, true);
}
#endif

GLFWOpenGLWindow::GLFWOpenGLWindow(int w, int h)
{
    ::glfwDefaultWindowHints();

    #ifdef __APPLE__
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    #endif
    
    m_glfwWindow = ::glfwCreateWindow(w, h, "", nullptr, nullptr);
    assert(m_glfwWindow);

    glfwMakeContextCurrent(m_glfwWindow);

    static bool isGladInit = false;
    if (isGladInit == false)
    {
        if (!gladLoadGL())
           throw GLADInitError();
        isGladInit = true;
    }

    setupGLFWcallback();
}

}