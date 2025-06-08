/*
 * ---------------------------------------------------
 * VulkanCommandBuffer.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/07 14:34:01
 * ---------------------------------------------------
 */

#define VULKAN_HPP_NO_CONSTRUCTORS

#include "Vulkan/VulkanCommandBuffer.hpp"

#include <vulkan/vulkan.hpp>

namespace gfx
{

VulkanCommandBuffer::VulkanCommandBuffer(const VulkanDevice& device, const vk::CommandPool& pool)
{
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo = {
        .commandPool = pool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1
    };
    m_vkCommandBuffer = device.vkDevice().allocateCommandBuffers(commandBufferAllocateInfo).front();
}

}
