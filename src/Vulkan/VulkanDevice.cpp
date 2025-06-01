/*
 * ---------------------------------------------------
 * VulkanDevice.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/28 06:27:45
 * ---------------------------------------------------
 */

#define VULKAN_HPP_NO_CONSTRUCTORS

#include "Graphics/Device.hpp"

#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    #include <cstdint>
    #include <cassert>
    #include <map>
    namespace ext = std;
#endif

namespace gfx
{

// get the list of queue family and the number of queue in each family required to satisfy the queue capabilities in Device::Descriptor
ext::map<QueueFamily, uint32_t> getQueueFamilyRequirements(const VulkanPhysicalDevice& phyDevice, const Device::Descriptor& desc)
{
    ext::map<QueueFamily, uint32_t> requirements;
    ext::vector<QueueFamily> queueFamilies = phyDevice.getQueueFamilies();

    for (auto& [capability, count] : desc.queues)
    {
        uint32_t remaingCount = count;
        for (auto& family : queueFamilies)
        {
            if (family.isCapableOf(capability, phyDevice) && family.count > 0)
            {
                uint32_t nbrTaken = ext::min(family.count, remaingCount); // nbr of queues taken from the family
                requirements[family] += nbrTaken;
                family.count -= nbrTaken;
                remaingCount -= nbrTaken;
                if (remaingCount == 0)
                    break;
            }
        }
        assert(remaingCount == 0); // should not hapend because checked in `isSuitable`
    }
    return requirements;
}

VulkanDevice::VulkanDevice(const VulkanPhysicalDevice& phyDevice, const Device::Descriptor& desc)
    : m_physicalDevice(&phyDevice)
{
    ext::map<QueueFamily, uint32_t> queueFamilyRequirements = getQueueFamilyRequirements(phyDevice, desc);
    ext::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    ext::vector<ext::vector<float>> queuePriorities = ext::vector<ext::vector<float>>(queueFamilyRequirements.size());

    for (int i = 0; auto& [family, count] : getQueueFamilyRequirements(phyDevice, desc))
    {
        auto& queuePriority = queuePriorities[i++];
        queuePriority.resize(count);
        queueCreateInfos.push_back({.queueFamilyIndex = family.index,
                                    .queueCount = count,
                                    .pQueuePriorities = &queuePriority[0]});
        m_queues[family] = ext::vector<vk::Queue>(count);
    }

    std::vector<const char*> deviceExtensions;

#if defined(__APPLE__)
    deviceExtensions.push_back("VK_KHR_portability_subset");
#endif

    for (auto& [capability, _] : desc.queues)
    {
        if (capability.surfaceSupport.empty() == false)
        {
            deviceExtensions.push_back(vk::KHRSwapchainExtensionName);
            break;
        }
    }

    vk::PhysicalDeviceFeatures deviceFeatures{};

    vk::DeviceCreateInfo deviceCreateInfo = {
        .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.data(),
        .pEnabledFeatures = &deviceFeatures};
    deviceCreateInfo.setQueueCreateInfos(queueCreateInfos);

    m_vkDevice = phyDevice.vkDevice().createDevice(deviceCreateInfo);

    for (auto& [family, queues] : m_queues)
    {
        for (int i = 0; auto& queue : queues)
            queue = m_vkDevice.getQueue(family.index, i++);
    }
}

VulkanDevice::~VulkanDevice()
{
    m_vkDevice.destroy();
}

} // namespace gfx
