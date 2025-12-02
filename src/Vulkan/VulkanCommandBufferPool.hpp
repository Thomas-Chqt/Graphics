/*
 * ---------------------------------------------------
 * VulkanCommandBufferPool.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/17 14:49:52
 * ---------------------------------------------------
 */

#ifndef VULKANCOMMANDBUFFERPOOL_HPP
#define VULKANCOMMANDBUFFERPOOL_HPP

#include "Graphics/CommandBufferPool.hpp"
#include "Graphics/CommandBuffer.hpp"

#include "Vulkan/QueueFamily.hpp"
#include "Vulkan/VulkanCommandBuffer.hpp"

namespace gfx
{

class VulkanDevice;

class VulkanCommandBufferPool : public CommandBufferPool
{
public:
    VulkanCommandBufferPool() = delete;
    VulkanCommandBufferPool(const VulkanCommandBufferPool&) = delete;
    VulkanCommandBufferPool(VulkanCommandBufferPool&&) = delete;

    VulkanCommandBufferPool(const VulkanDevice*, const QueueFamily&);

    std::shared_ptr<CommandBuffer> get() override;
    void reset() override;

    ~VulkanCommandBufferPool() override = default;

private:
    const VulkanDevice* m_device;

    std::shared_ptr<vk::CommandPool> m_vkCommandPool; // buffers can outlive the pool, so the vkCommandPool need to be kept alive

    std::deque<std::shared_ptr<VulkanCommandBuffer>> m_availableCommandBuffers;
    std::deque<std::shared_ptr<VulkanCommandBuffer>> m_usedCommandBuffers;

public:
    VulkanCommandBufferPool& operator=(const VulkanCommandBufferPool&) = delete;
    VulkanCommandBufferPool& operator=(VulkanCommandBufferPool&&) = delete;
};

} // namespace gfx

#endif // VULKANCOMMANDBUFFERPOOL_HPP
