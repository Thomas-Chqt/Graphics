/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/06 16:59:24
 * ---------------------------------------------------
 */

#include <GLFW/glfw3.h>
#include "Graphics/Instance.hpp"

#if defined(GFX_USE_UTILSCPP)
    #include "UtilsCPP/memory.hpp"
    namespace ext = utl;
#else
    #include <memory>
    namespace ext = std;
#endif

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main()
{
    glfwInit();

    ext::unique_ptr<gfx::Instance> instance = gfx::Instance::newInstance(gfx::Instance::Descriptor{});

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GLFW Window", nullptr, nullptr);

    while (glfwWindowShouldClose(window) == false)
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
