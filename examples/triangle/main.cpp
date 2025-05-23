/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/06 16:59:24
 * ---------------------------------------------------
 */

#include <GLFW/glfw3.h>
#include "Graphics/Device.hpp"
#include "UtilsCPP/UniquePtr.hpp"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main()
{
    glfwInit();

    utils::UniquePtr<gfx::Device> device = gfx::Device::createDevice();

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GLFW Window", nullptr, nullptr);

    while (glfwWindowShouldClose(window) == false)
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
