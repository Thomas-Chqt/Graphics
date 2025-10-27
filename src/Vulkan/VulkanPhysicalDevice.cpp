/*
 * ---------------------------------------------------
 * VulkanPhysicalDevice.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/30 14:52:32
 * ---------------------------------------------------
 */

#include "Vulkan/VulkanPhysicalDevice.hpp"

namespace gfx
{

bool VulkanPhysicalDevice::isSuitable(const VulkanDevice::Descriptor& desc) const
{
    if ((getQueueFamilies() | std::views::filter([&desc](auto f){ return f.hasCapabilities(desc.deviceDescriptor->queueCaps); })).empty())
        return false;

    auto requiredExtension = desc.deviceExtensions | std::views::filter([&](const char* ext) {
        if (strcmp(ext, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME) == 0 && vk::PhysicalDevice::getProperties().apiVersion >= vk::ApiVersion13)
            return false;
        if (strcmp(ext, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME) == 0 && vk::PhysicalDevice::getProperties().apiVersion >= VK_API_VERSION_1_3)
            return false;
        return true;
    });

    if (suportExtensions(requiredExtension | std::ranges::to<std::vector>()) == false)
        return false;

    return true;
}

std::vector<QueueFamily> VulkanPhysicalDevice::getQueueFamilies() const
{
    std::vector<QueueFamily> queueFamilies;
    for (uint32_t i = 0; auto& familyProp : vk::PhysicalDevice::getQueueFamilyProperties())
    {
        QueueFamily queueFamily(familyProp);
        queueFamily.index = i;
        queueFamily.device = this;
        queueFamilies.push_back(queueFamily);
        i++;
    }
    return queueFamilies;
}

bool VulkanPhysicalDevice::suportExtensions(const std::vector<const char*>& extensionNames) const
{
    std::vector<vk::ExtensionProperties> availableExtensions = vk::PhysicalDevice::enumerateDeviceExtensionProperties();
    std::set<std::string> availableExtensionNames;
    for (auto extensionName : availableExtensions | std::views::transform([](auto& e){ return e.extensionName; }))
        availableExtensionNames.insert(extensionName);
    if (std::ranges::all_of(extensionNames, [&availableExtensionNames](const char* name){ return availableExtensionNames.contains(name); }))
        return true;
    return false;
}

uint32_t VulkanPhysicalDevice::findSuitableMemoryTypeIdx(vk::MemoryPropertyFlags properties, uint32_t mask) const
{
    vk::PhysicalDeviceMemoryProperties memProperties = vk::PhysicalDevice::getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((mask & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }
    throw std::runtime_error("no suitable memory type found");
}

}
