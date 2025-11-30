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

VulkanCommandBufferPool::VulkanCommandBufferPool(const VulkanDevice* device, const QueueFamily& queueFamily)
    : m_device(device)
{
    auto commandPoolCreateInfo = vk::CommandPoolCreateInfo{}
        .setQueueFamilyIndex(queueFamily.index);

    m_vkCommandPool = std::shared_ptr<vk::CommandPool>(
        new vk::CommandPool(m_device->vkDevice().createCommandPool(commandPoolCreateInfo)), // NOLINT(cppcoreguidelines-owning-memory)
        [device=m_device](vk::CommandPool* pool){
            device->vkDevice().destroyCommandPool(*pool);
            delete pool; // NOLINT(cppcoreguidelines-owning-memory)
        }
    );
}

std::shared_ptr<CommandBuffer> VulkanCommandBufferPool::get()
{
    std::shared_ptr<VulkanCommandBuffer> commandBuffer;
    if (m_availableCommandBuffers.empty() == false) {
        commandBuffer = std::move(m_availableCommandBuffers.front());
        m_availableCommandBuffers.pop_front();
    }
    else {
        commandBuffer = std::make_shared<VulkanCommandBuffer>(m_device, m_vkCommandPool);
    }
    m_usedCommandBuffers.push_back(commandBuffer);
    commandBuffer->begin();
    return commandBuffer;
}

void VulkanCommandBufferPool::reset()
{
    m_device->vkDevice().resetCommandPool(*m_vkCommandPool);
    for (auto& commandBuffer : m_usedCommandBuffers) {
        commandBuffer->reuse();
        m_availableCommandBuffers.push_back(std::move(commandBuffer));
    }
    m_usedCommandBuffers.clear();
}

}
