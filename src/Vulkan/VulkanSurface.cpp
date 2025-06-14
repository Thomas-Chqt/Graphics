/*
 * ---------------------------------------------------
 * VulkanSurface.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/30 07:02:58
 * ---------------------------------------------------
 */

#include "Graphics/Device.hpp"

#include "Vulkan/VulkanSurface.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"
#include "Vulkan/VulkanEnums.hpp"

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
VulkanSurface::VulkanSurface(vk::Instance& instance, GLFWwindow* glfwWindow) : m_vkInstance(&instance)
{
    VkSurfaceKHR rawSurface;
    if (glfwCreateWindowSurface(static_cast<VkInstance>(instance), glfwWindow, nullptr, (uint64_t*)&rawSurface) != VK_SUCCESS)
        throw ext::runtime_error("unable to create the surface");
    m_vkSurface = rawSurface;
}
#endif

const ext::set<PixelFormat> VulkanSurface::supportedPixelFormats(const Device& _device) const
{
    const VulkanDevice& device = dynamic_cast<const VulkanDevice&>(_device);

    ext::set<PixelFormat> pixelFormats;
    for (const vk::SurfaceFormatKHR& format : device.physicalDevice().getSurfaceFormatsKHR(m_vkSurface))
    {
        switch (format.format)
        {
        case vk::Format::eB8G8R8A8Unorm:
        case vk::Format::eB8G8R8A8Srgb:
            pixelFormats.insert(toPixelFormat(format.format));
        default:
            break;
        }
    }
    return pixelFormats;
}

const ext::set<PresentMode> VulkanSurface::supportedPresentModes(const Device& _device) const
{
    const VulkanDevice& device = dynamic_cast<const VulkanDevice&>(_device);

    ext::set<PresentMode> modes;
    for (const vk::PresentModeKHR& mode : device.physicalDevice().getSurfacePresentModesKHR(m_vkSurface))
    {
        switch (mode)
        {
        case vk::PresentModeKHR::eFifo:
        case vk::PresentModeKHR::eMailbox:
            modes.insert(toPresentMode(mode));
        default:
            break;
        }
    }
    return modes;
}

VulkanSurface::~VulkanSurface()
{
    assert(m_vkInstance);
    m_vkInstance->destroySurfaceKHR(m_vkSurface);
}

}
