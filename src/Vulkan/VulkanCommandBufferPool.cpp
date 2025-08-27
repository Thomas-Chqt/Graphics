/*
 * ---------------------------------------------------
 * VulkanCommandBufferPool.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/17 15:02:58
 * ---------------------------------------------------
 */

#include "Vulkan/VulkanCommandBufferPool.hpp"
#include "Vulkan/VulkanCommandBuffer.hpp"
#include "Vulkan/VulkanDevice.hpp"

namespace gfx
{

VulkanCommandBufferPool::VulkanCommandBufferPool(VulkanCommandBufferPool&& other) noexcept
    : m_device(ext::exchange(other.m_device, nullptr)),
      m_queueFamily(ext::exchange(other.m_queueFamily, QueueFamily{})),
      m_pools(ext::move(other.m_pools))
{
    other.m_frontPool = &other.m_pools[0];
    other.m_backPool = &other.m_pools[1];
}

VulkanCommandBufferPool::VulkanCommandBufferPool(const VulkanDevice* device, const QueueFamily& queueFamily)
    : m_device(device), m_queueFamily(queueFamily)
{
    auto commandPoolCreateInfo = vk::CommandPoolCreateInfo{}
        .setQueueFamilyIndex(m_queueFamily.index);

    m_pools[0].vkCommandPool = m_device->vkDevice().createCommandPool(commandPoolCreateInfo);
    m_pools[1].vkCommandPool = m_device->vkDevice().createCommandPool(commandPoolCreateInfo);
}

VulkanCommandBuffer& VulkanCommandBufferPool::get()
{
    assert(m_device);
    VulkanCommandBuffer commandBuffer;
    if (m_frontPool->free.empty() == false) {
        commandBuffer = ext::move(m_frontPool->free.front());
        m_frontPool->free.pop_front();
    }
    else {
        auto commandBufferAllocateInfo = vk::CommandBufferAllocateInfo{}
            .setCommandPool(m_frontPool->vkCommandPool)
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(1);
        commandBuffer = VulkanCommandBuffer(
            ext::move(m_device->vkDevice().allocateCommandBuffers(commandBufferAllocateInfo).front()),
            m_queueFamily);
    }
    commandBuffer.begin();
    return m_frontPool->inUse.emplace_back(std::move(commandBuffer));
}

void VulkanCommandBufferPool::swapPools()
{
    ext::swap(m_frontPool, m_backPool);
}

void VulkanCommandBufferPool::reset()
{
    assert(m_device);
    while(m_backPool->inUse.empty() == false) {
        m_backPool->inUse.front().clear();
        m_backPool->free.emplace_back(ext::move(m_backPool->inUse.front()));
        m_backPool->inUse.pop_front();
    }
    m_device->vkDevice().resetCommandPool(m_backPool->vkCommandPool);
}

void VulkanCommandBufferPool::clear()
{
    for (auto& pool : m_pools)
    {
        for (auto& buff : pool.inUse) {
            buff.clear();
        }
        if (pool.vkCommandPool) {
            assert(m_device);
            m_device->vkDevice().destroyCommandPool(pool.vkCommandPool);
            pool.vkCommandPool = VK_NULL_HANDLE;
        }
        pool.inUse.clear();
        pool.free.clear();
    }
    m_frontPool = &m_pools[0];
    m_backPool = &m_pools[1];
    m_queueFamily = QueueFamily{};
    m_device = nullptr;
}

VulkanCommandBufferPool::~VulkanCommandBufferPool()
{
    clear();
}

VulkanCommandBufferPool& VulkanCommandBufferPool::operator=(VulkanCommandBufferPool&& other) noexcept
{
    if (this != &other)
    {
        clear();
        m_device = ext::exchange(other.m_device, nullptr);
        m_queueFamily = ext::exchange(other.m_queueFamily, QueueFamily{});
        m_pools = ext::move(other.m_pools);

        other.m_frontPool = &other.m_pools[0];
        other.m_backPool = &other.m_pools[1];
    }
    return *this;
}

}
