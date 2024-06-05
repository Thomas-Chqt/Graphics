/*
 * ---------------------------------------------------
 * IMockGLFW.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/04 00:13:58
 * ---------------------------------------------------
 */

#include "GLFW/IMockGLFW.hpp"

namespace gfx_tests
{
    IMockGLFW* IMockGLFW::s_instance = nullptr;
}

GLFWAPI int glfwInit(void) { 
    return gfx_tests::IMockGLFW::s_instance->glfwInit();

}

GLFWAPI GLFWerrorfun glfwSetErrorCallback(GLFWerrorfun callback) {
    return gfx_tests::IMockGLFW::s_instance->glfwSetErrorCallback(callback);
}

GLFWAPI void glfwTerminate(void) {
    return gfx_tests::IMockGLFW::s_instance->glfwTerminate();
}

GLFWAPI void glfwDefaultWindowHints(void) {
    return gfx_tests::IMockGLFW::s_instance->glfwDefaultWindowHints();
}

GLFWAPI void glfwWindowHint(int hint, int value) {
    return gfx_tests::IMockGLFW::s_instance->glfwWindowHint(hint, value);
}

GLFWAPI GLFWwindow* glfwCreateWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share) {
    return gfx_tests::IMockGLFW::s_instance->glfwCreateWindow(width, height, title, monitor, share);
}

GLFWAPI void glfwGetFramebufferSize(GLFWwindow* window, int* width, int* height) {
    return gfx_tests::IMockGLFW::s_instance->glfwGetFramebufferSize(window, width, height);
}

#ifdef METAL_ENABLED
    GLFWAPI id glfwGetCocoaWindow(GLFWwindow* window) {
        return gfx_tests::IMockGLFW::s_instance->glfwGetCocoaWindow(window);
    }
#endif