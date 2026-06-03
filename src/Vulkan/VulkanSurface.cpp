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

namespace gfx
{

VulkanSurface::VulkanSurface(const vk::Instance& instance, VkSurfaceKHR surface)
    : m_vkInstance(&instance), m_vkSurface(surface)
{
}

const std::set<PixelFormat> VulkanSurface::supportedPixelFormats(const Device& _device) const
{
    const auto& device = dynamic_cast<const VulkanDevice&>(_device);

    std::set<PixelFormat> pixelFormats;
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

const std::set<PresentMode> VulkanSurface::supportedPresentModes(const Device& _device) const
{
    const auto& device = dynamic_cast<const VulkanDevice&>(_device);

    std::set<PresentMode> modes;
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
