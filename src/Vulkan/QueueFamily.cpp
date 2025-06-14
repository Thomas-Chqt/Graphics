/*
 * ---------------------------------------------------
 * QueueFamily.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/13 16:41:53
 * ---------------------------------------------------
 */

#include "Graphics/QueueCapabilities.hpp"

#include "Vulkan/QueueFamily.hpp"
#include "Vulkan/VulkanSurface.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"

namespace gfx
{

bool QueueFamily::hasCapabilities(const QueueCapabilities& capabilities) const
{
    if (capabilities.graphics && (vk::QueueFamilyProperties::queueFlags & vk::QueueFlagBits::eGraphics) == vk::QueueFlagBits{})
        return false;
    if (capabilities.compute && (vk::QueueFamilyProperties::queueFlags & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits{})
        return false;
    if (capabilities.transfer && (vk::QueueFamilyProperties::queueFlags & vk::QueueFlagBits::eTransfer) == vk::QueueFlagBits{})
        return false;
    for (auto& surface : capabilities.present)
    {
        if (device->getSurfaceSupportKHR(index, dynamic_cast<const VulkanSurface&>(*surface).vkSurface()) == false)
            return false;
    }
    return true;
}

}
