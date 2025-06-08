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
#include "Graphics/Swapchain.hpp"
#include "Graphics/RenderPass.hpp"

#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"
#include "Vulkan/VulkanSwapchain.hpp"
#include "Vulkan/VulkanRenderPass.hpp"
#include "Vulkan/VulkanCommandBuffer.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    #include <cstdint>
    #include <cassert>
    #include <map>
    #include <memory>
    #include <thread>
    #include <mutex>
    #include <functional>
    #include <utility>
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

ext::unique_ptr<RenderPass> VulkanDevice::newRenderPass(const RenderPass::Descriptor& desc) const
{
    return ext::make_unique<VulkanRenderPass>(*this, desc);
}

ext::unique_ptr<Swapchain> VulkanDevice::newSwapchain(const Swapchain::Descriptor& desc) const
{
    return ext::make_unique<VulkanSwapchain>(*this, desc);
}

ext::unique_ptr<CommandBuffer> VulkanDevice::newCommandBuffer()
{
    assert(m_queues.size() == 1);

    static thread_local ext::map<uintptr_t, vk::CommandPool*> commandPools = {
        ext::make_pair(uintptr_t(this), &makeThreadCommandPool(ext::this_thread::get_id()))
    };

    if (commandPools.contains(uintptr_t(this)) == false)
        commandPools[uintptr_t(this)] = &makeThreadCommandPool(ext::this_thread::get_id());
    
    return ext::make_unique<VulkanCommandBuffer>(*this, *commandPools[uintptr_t(this)]);
}

vk::CommandPool& VulkanDevice::makeThreadCommandPool(ext::thread::id id)
{
    assert(m_queues.size() == 1);
    auto& [family, queue] = *m_queues.begin();
    vk::CommandPoolCreateInfo commandPoolCreateInfo = {
        .queueFamilyIndex = family.index
    };
    ext::unique_ptr<vk::CommandPool, ext::function<void(vk::CommandPool*)>> newPool(new vk::CommandPool(m_vkDevice.createCommandPool(commandPoolCreateInfo)), [this](vk::CommandPool* pool){ 
        m_vkDevice.destroyCommandPool(*pool);
        delete pool;
    });
    ext::lock_guard<ext::mutex> lock(m_commandPoolsMutex);
    m_commandPools[family][m_frameIndex][id] = ext::move(newPool);
    return *m_commandPools[family][m_frameIndex][id].get();
}

VulkanDevice::~VulkanDevice()
{
    m_commandPools.clear();
    m_vkDevice.destroy();
}

} // namespace gfx
