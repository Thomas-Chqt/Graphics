/*
 * ---------------------------------------------------
 * VulkanSurface.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/30 07:02:58
 * ---------------------------------------------------
 */

#include "Vulkan/VulkanSurface.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <stdexcept>
    #include <cassert>
    #include <cstdint>
    namespace ext = std;
#endif

#if defined(GFX_GLFW_ENABLED)
    #include <dlLoad/dlLoad.h>
    class GLFWwindow;
    #define glfwCreateWindowSurface ((VkResult (*)(void*, GLFWwindow*, const VkAllocationCallbacks*, uint64_t*))::getSym(DL_DEFAULT, "glfwCreateWindowSurface"))
#endif

namespace gfx
{

#if defined(GFX_GLFW_ENABLED)
VulkanSurface::VulkanSurface(vk::Instance& instance, GLFWwindow* glfwWindow) : m_vkInstance(instance)
{
    VkSurfaceKHR rawSurface;
    if (glfwCreateWindowSurface(static_cast<VkInstance>(m_vkInstance), glfwWindow, nullptr, (uint64_t*)&rawSurface) != VK_SUCCESS)
        throw ext::runtime_error("unable to create the surface");
    m_vkSurface = rawSurface;
}
#endif

VulkanSurface::~VulkanSurface()
{
    m_vkInstance.destroySurfaceKHR(m_vkSurface);
}

}
