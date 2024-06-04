/*
 * ---------------------------------------------------
 * IMockGLFW.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/04 00:13:58
 * ---------------------------------------------------
 */

#include "IMockGLFW.hpp"

#include "GLFW/glfw3.h"

namespace gfx_test
{
    IMockGLFW* IMockGLFW::s_instance = nullptr;
}

GLFWAPI int          glfwInit             (void)                  { return gfx_test::IMockGLFW::s_instance->glfwInit(); }
GLFWAPI GLFWerrorfun glfwSetErrorCallback (GLFWerrorfun callback) { return gfx_test::IMockGLFW::s_instance->glfwSetErrorCallback(callback); }
GLFWAPI void         glfwTerminate        (void)                  { return gfx_test::IMockGLFW::s_instance->glfwTerminate(); }

