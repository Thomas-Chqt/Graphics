/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/06 16:59:24
 * ---------------------------------------------------
 */

#include "Graphics/Instance.hpp"
#include "Graphics/PhysicalDevice.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Surface.hpp"

#include <GLFW/glfw3.h>

#if defined(GFX_USE_UTILSCPP)
    #include "UtilsCPP/memory.hpp"
    namespace ext = utl;
#else
    #include <memory>
    #include <vector>
    #include <iostream>
    #include <cassert>
    namespace ext = std;
#endif

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GLFW Window", nullptr, nullptr);

    ext::unique_ptr<gfx::Instance> instance = gfx::Instance::newInstance(gfx::Instance::Descriptor{});
    assert(instance);

    ext::unique_ptr<gfx::Surface> surface = instance->createSurface(window);
    assert(surface);

    ext::vector<ext::unique_ptr<gfx::PhysicalDevice>> phyDevices = instance->listPhysicalDevices();
    assert(phyDevices.empty() == false);

    for (auto& device : phyDevices)
    {
        ext::cout << device->name() << ext::endl;   
    }
    
    ext::unique_ptr<gfx::Device> device = instance->newDevice(gfx::Device::Descriptor{}, *phyDevices[0]);
    assert(device);

    while (glfwWindowShouldClose(window) == false)
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
