/*
 * ---------------------------------------------------
 * IMockGLFW.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/04 12:11:18
 * ---------------------------------------------------
 */

#ifndef IMOCKGLFW_HPP
# define IMOCKGLFW_HPP

#ifdef METAL_ENABLED
    #define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include "glfw3.h"

#ifdef METAL_ENABLED
    #include "glfw3native.h" // IWYU pragma: keep
#endif

namespace gfx_tests
{

class IMockGLFW
{
public:
    virtual ~IMockGLFW() {};

    virtual int glfwInit(void) = 0;
    virtual GLFWerrorfun glfwSetErrorCallback(GLFWerrorfun callback) = 0;
    virtual void glfwTerminate(void) = 0;
    virtual void glfwDefaultWindowHints(void) = 0;
    virtual void glfwWindowHint(int, int) = 0;
    virtual GLFWwindow* glfwCreateWindow(int, int, const char*, GLFWmonitor*, GLFWwindow*) = 0;
    virtual void glfwGetFramebufferSize(GLFWwindow*, int*, int*) = 0;
    
    #ifdef METAL_ENABLED
        virtual id glfwGetCocoaWindow (GLFWwindow*) = 0;
    #endif

    static IMockGLFW* s_instance;
};

}

#endif // IMOCKGLFW_HPP