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
#include "Graphics/Framebuffer.hpp"

#include "Vulkan/QueueFamily.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
#else
#endif

namespace gfx
{

class VulkanCommandBuffer : public CommandBuffer
{
public:
    VulkanCommandBuffer() = default;
    VulkanCommandBuffer(const VulkanCommandBuffer&) = delete;
    VulkanCommandBuffer(VulkanCommandBuffer&&) = delete;

    VulkanCommandBuffer(vk::CommandBuffer&&, const QueueFamily&, bool useDynamicRenderingExt);

    void beginRenderPass(const Framebuffer&) override;
    void endRenderPass(void) override;

    const vk::CommandBuffer& vkCommandBuffer(void) const { return m_vkCommandBuffer; }
    vk::CommandBuffer& vkCommandBuffer(void) { return m_vkCommandBuffer; }

    ~VulkanCommandBuffer() = default;

private:
    vk::CommandBuffer m_vkCommandBuffer;
    QueueFamily m_queueFamily;
    bool m_useDynamicRenderingExt;
    bool m_isBegin = false;

public:
    VulkanCommandBuffer& operator = (const VulkanCommandBuffer&) = delete;
    VulkanCommandBuffer& operator = (VulkanCommandBuffer&&) = delete;
};

}

#endif // VULKANCOMMANDBUFFER_HPP
