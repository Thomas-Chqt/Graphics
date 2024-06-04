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

#include "GLFW/glfw3.h"

namespace gfx_test
{

class IMockGLFW
{
public:
    virtual ~IMockGLFW() {};

    virtual int glfwInit(void) = 0;
    virtual GLFWerrorfun glfwSetErrorCallback(GLFWerrorfun callback) = 0;
    virtual void glfwTerminate(void) = 0;

    static IMockGLFW* s_instance;
};

}

#endif // IMOCKGLFW_HPP