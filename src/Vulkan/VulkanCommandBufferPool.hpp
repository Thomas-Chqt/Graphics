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

#include "Vulkan/QueueFamily.hpp"
#include "Vulkan/VulkanCommandBuffer.hpp"

namespace gfx
{

class VulkanDevice;

class VulkanCommandBufferPool
{
public:
    VulkanCommandBufferPool() = default;
    VulkanCommandBufferPool(const VulkanCommandBufferPool&) = delete;
    VulkanCommandBufferPool(VulkanCommandBufferPool&&) noexcept;

    VulkanCommandBufferPool(const VulkanDevice*, const QueueFamily&);

    VulkanCommandBuffer& get();
    void swapPools(); // swap back and front pools
    void reset(); // reset the back pool
    void clear();

    ~VulkanCommandBufferPool();

private:
    struct PoolData
    {
        vk::CommandPool vkCommandPool;
        ext::deque<VulkanCommandBuffer> free;
        ext::deque<VulkanCommandBuffer> inUse;
    };

    const VulkanDevice* m_device = nullptr;
    QueueFamily m_queueFamily;

    ext::array<PoolData, 2> m_pools;

    PoolData* m_frontPool = &m_pools[0];
    PoolData* m_backPool = &m_pools[1];

public:
    VulkanCommandBufferPool& operator=(const VulkanCommandBufferPool&) = delete;
    VulkanCommandBufferPool& operator=(VulkanCommandBufferPool&&) noexcept;
};

} // namespace gfx

#endif // VULKANCOMMANDBUFFERPOOL_HPP
