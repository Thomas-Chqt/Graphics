/*
 * ---------------------------------------------------
 * VulkanPhysicalDevice.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/30 14:52:32
 * ---------------------------------------------------
 */

#include "Vulkan/VulkanPhysicalDevice.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
#else
    #include <vector>
    #include <ranges>
    #include <set>
    #include <algorithm>
    #include <string>
    namespace ext = std;
#endif

namespace gfx
{

bool VulkanPhysicalDevice::isSuitable(const VulkanDevice::Descriptor& desc) const
{
    if ((getQueueFamilies() | ext::views::filter([&desc](auto f){ return f.hasCapabilities(desc.deviceDescriptor->queueCaps); })).empty())
        return false;
    if (suportExtensions(desc.deviceExtensions) == false)
        return false;
    return true;
}

ext::vector<QueueFamily> VulkanPhysicalDevice::getQueueFamilies() const
{
    ext::vector<QueueFamily> queueFamilies;
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

bool VulkanPhysicalDevice::suportExtensions(const ext::vector<const char*>& extensionNames) const
{
    ext::vector<vk::ExtensionProperties> availableExtensions = vk::PhysicalDevice::enumerateDeviceExtensionProperties();
    ext::set<ext::string> availableExtensionNames;
    for (auto extensionName : availableExtensions | ext::views::transform([](auto& e){ return e.extensionName; }))
        availableExtensionNames.insert(extensionName);
    if (ext::ranges::all_of(extensionNames, [&availableExtensionNames](const char* name){ return availableExtensionNames.contains(name); }))
        return true;
    return false;
}

}
