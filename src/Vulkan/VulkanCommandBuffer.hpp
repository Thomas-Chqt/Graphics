/*
 * ---------------------------------------------------
 * VulkanCommandBuffer.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/07 14:30:59
 * ---------------------------------------------------
 */

#ifndef VULKANCOMMANDBUFFER_HPP
#define VULKANCOMMANDBUFFER_HPP

#include "Graphics/CommandBuffer.hpp"

#include "Vulkan/VulkanDevice.hpp"

#include <vulkan/vulkan.hpp>

namespace gfx
{

class VulkanCommandBuffer : public CommandBuffer
{
public:
    VulkanCommandBuffer() = delete;
    VulkanCommandBuffer(const VulkanCommandBuffer&) = delete;
    VulkanCommandBuffer(VulkanCommandBuffer&&)      = delete;
    
    VulkanCommandBuffer(const VulkanDevice&, const vk::CommandPool&);

    ~VulkanCommandBuffer() = default;

private:
    vk::CommandBuffer m_vkCommandBuffer;
    
public:
    VulkanCommandBuffer& operator = (const VulkanCommandBuffer&) = delete;
    VulkanCommandBuffer& operator = (VulkanCommandBuffer&&)      = delete;
};

}

#endif // VULKANCOMMANDBUFFER_HPP
