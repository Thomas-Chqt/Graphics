/*
 * ---------------------------------------------------
 * VulkanPhysicalDevice.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/30 14:52:32
 * ---------------------------------------------------
 */

#include "Graphics/Enums.hpp"

#include "Vulkan/VulkanPhysicalDevice.hpp"
#include "Graphics/Surface.hpp"
#include "Vulkan/VulkanSurface.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <string>
    #include <cstdint>
    #include <vector>
    namespace ext = std;
#endif

namespace gfx
{

bool QueueFamily::isCapableOf(const QueueCapability& capability, const VulkanPhysicalDevice& phyDevice) const
{
    if ((flags & capability.flags) != capability.flags)
        return false;
    for (Surface* surface : capability.surfaceSupport)
    {
        const VulkanSurface* vkSurface = dynamic_cast<const VulkanSurface*>(surface);
        if (vkSurface == nullptr)
            return false;
        if (phyDevice.vkDevice().getSurfaceSupportKHR(index, vkSurface->vkSurface()) == false)
            return false;
    }
    return true;
}

VulkanPhysicalDevice::VulkanPhysicalDevice(vk::PhysicalDevice phyDevice)
    : m_vkPhyDevice(phyDevice)
{
}

ext::string VulkanPhysicalDevice::name() const
{
    return "";
}

bool VulkanPhysicalDevice::isSuitable(const Device::Descriptor&) const
{
    return true; // TODO : real check
}

ext::vector<QueueFamily> VulkanPhysicalDevice::getQueueFamilies() const
{
    ext::vector<QueueFamily> queueFamilies;
    ext::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_vkPhyDevice.getQueueFamilyProperties();
    for (uint32_t i = 0; auto& familyProp : queueFamilyProperties)
    {
        QueueCapabilityFlag flags;
        if ((familyProp.queueFlags & vk::QueueFlagBits::eGraphics) != vk::QueueFlagBits(0))
            flags = flags | QueueCapabilityFlag::Graphics;
        if ((familyProp.queueFlags & vk::QueueFlagBits::eCompute) != vk::QueueFlagBits(0))
            flags = flags | QueueCapabilityFlag::Compute;
        if ((familyProp.queueFlags & vk::QueueFlagBits::eTransfer) != vk::QueueFlagBits(0))
            flags = flags | QueueCapabilityFlag::Transfer;
        queueFamilies.push_back(QueueFamily{
            .flags = flags,
            .count = familyProp.queueCount,
            .index = i
        });
        i++;
    }
    return queueFamilies;
}

}
