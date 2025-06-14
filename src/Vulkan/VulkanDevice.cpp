/*
 * ---------------------------------------------------
 * VulkanDevice.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/28 06:27:45
 * ---------------------------------------------------
 */

#define VULKAN_HPP_NO_CONSTRUCTORS

#include "Graphics/RenderPass.hpp"
#include "Graphics/Swapchain.hpp"
#include "Graphics/CommandBuffer.hpp"

#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"
#include "Vulkan/VulkanRenderPass.hpp"
#include "Vulkan/VulkanSwapchain.hpp"
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
    #include <ranges>
    namespace ext = std;
#endif

namespace gfx
{

VulkanDevice::VulkanDevice(const VulkanPhysicalDevice& phyDevice, const VulkanDevice::Descriptor& desc)
    : m_physicalDevice(&phyDevice)
{
    assert((phyDevice.getQueueFamilies() | ext::views::filter([&desc](auto f){ return f.hasCapabilities(desc.deviceDescriptor->queueCaps); })).empty() == false);

    m_queueFamily = (phyDevice.getQueueFamilies() | ext::views::filter([&desc](auto f){ return f.hasCapabilities(desc.deviceDescriptor->queueCaps); })).front();

    float queuePriority = 1.0f;
    vk::DeviceQueueCreateInfo queueCreateInfo = {
        .queueFamilyIndex = m_queueFamily.index,
        .queueCount = 1,                      
        .pQueuePriorities = &queuePriority};

    vk::PhysicalDeviceFeatures deviceFeatures{};

    vk::DeviceCreateInfo deviceCreateInfo = {
        .enabledExtensionCount = static_cast<uint32_t>(desc.deviceExtensions.size()),
        .ppEnabledExtensionNames = desc.deviceExtensions.data(),
        .pEnabledFeatures = &deviceFeatures,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueCreateInfo};

    m_vkDevice = phyDevice.createDevice(deviceCreateInfo);
    m_queue = m_vkDevice.getQueue(m_queueFamily.index, 0);
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
    static thread_local ext::map<uintptr_t, CommandPools*> commandPools;

    if (commandPools.contains(uintptr_t(this)) == false)
        commandPools[uintptr_t(this)] = &makeThreadCommandPools(ext::this_thread::get_id());
    
    return ext::make_unique<VulkanCommandBuffer>(*this, (*commandPools[uintptr_t(this)])[m_frameIndex]);
}

CommandPools& VulkanDevice::makeThreadCommandPools(ext::thread::id id)
{
    vk::CommandPoolCreateInfo commandPoolCreateInfo = {
        .queueFamilyIndex = m_queueFamily.index
    };
    ext::lock_guard<ext::mutex> lock(m_commandPoolsMutex);
    m_commandPools[id] = {
        m_vkDevice.createCommandPool(commandPoolCreateInfo),
        m_vkDevice.createCommandPool(commandPoolCreateInfo),
        m_vkDevice.createCommandPool(commandPoolCreateInfo)
    };
    return m_commandPools[id];
}

VulkanDevice::~VulkanDevice()
{
    for (auto& [_, pools] : m_commandPools)
    {
        for (auto& pool : pools)
            m_vkDevice.destroyCommandPool(pool);
    }
    m_vkDevice.destroy();
}

} // namespace gfx
