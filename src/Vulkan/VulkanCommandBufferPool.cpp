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
#include <cassert>
#include <memory>

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

std::unique_ptr<CommandBuffer> VulkanCommandBufferPool::get()
{
    std::unique_ptr<VulkanCommandBuffer> commandBuffer;
    if (m_availableCommandBuffers.empty() == false) {
        commandBuffer = std::move(m_availableCommandBuffers.front());
        m_availableCommandBuffers.pop_front();
    }
    else {
        commandBuffer = std::make_unique<VulkanCommandBuffer>(m_device, m_vkCommandPool, this);
    }
    assert(m_usedCommandBuffers.contains(commandBuffer.get()) == false);
    // in vulkan, command buffers can be reused so the buffers in m_availableCommandBuffers already have a vk command buffer
    m_usedCommandBuffers.insert(commandBuffer.get());
    commandBuffer->begin();
    return commandBuffer;
}

void VulkanCommandBufferPool::release(std::unique_ptr<CommandBuffer>&& aCommandBuffer) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
{
    auto* commandBuffer = dynamic_cast<VulkanCommandBuffer*>(aCommandBuffer.release());
    assert(commandBuffer);
    commandBuffer->reuse();
    // buffer cannot be reused directly, the pool need to be reset
    m_resetableCommandBuffers.push_back(std::unique_ptr<VulkanCommandBuffer>(commandBuffer));
    release(commandBuffer); // because reuse() dont call release
}

void VulkanCommandBufferPool::release(CommandBuffer* aCommandBuffer)
{
    auto* commandBuffer = dynamic_cast<VulkanCommandBuffer*>(aCommandBuffer);
    assert(commandBuffer);
    assert(m_usedCommandBuffers.contains(commandBuffer));
    m_usedCommandBuffers.erase(commandBuffer);
    if (m_usedCommandBuffers.empty()) {
        // no more used command buffers, the pool can be reset
        m_device->vkDevice().resetCommandPool(*m_vkCommandPool);
        // the pool is reset, command buffers can be reused
        for (auto& cmdBuffer : m_resetableCommandBuffers) {
            m_availableCommandBuffers.push_back(std::move(cmdBuffer));
        }
        m_resetableCommandBuffers.clear();
    }
}

std::unique_ptr<VulkanCommandBuffer> VulkanCommandBufferPool::getVulkan()
{
    std::unique_ptr<CommandBuffer> cmdBuffer = get();
    auto* vkCmdBuffer = dynamic_cast<VulkanCommandBuffer*>(cmdBuffer.release());
    assert(vkCmdBuffer);
    return std::unique_ptr<VulkanCommandBuffer>(vkCmdBuffer);
}

VulkanCommandBufferPool::~VulkanCommandBufferPool()
{
    // commandBuffers can outlive the pool, so clearing the pool
    // prevent from trying to release a buffer to a freed pool
    for (auto& cmdBuffer : m_usedCommandBuffers)
        cmdBuffer->clearSourcePool();
    for (auto& cmdBuffer : m_resetableCommandBuffers)
        cmdBuffer->clearSourcePool();
    for (auto& cmdBuffer : m_availableCommandBuffers)
        cmdBuffer->clearSourcePool();
}

}
