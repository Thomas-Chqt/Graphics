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

#include "Vulkan/VulkanFramebuffer.hpp"
#include "Vulkan/VulkanRenderPass.hpp"
#include "Vulkan/VulkanTexture.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

#if defined(GFX_USE_UTILSCPP)
#else
    #include <vector>
    #include <memory>
    #include <map>
    #include <utility>
    #include <optional>
    namespace ext = std;
#endif

namespace gfx
{

class VulkanDevice;

class VulkanCommandBuffer : public CommandBuffer
{
public:
    VulkanCommandBuffer() = default;
    VulkanCommandBuffer(const VulkanCommandBuffer&) = delete;
    VulkanCommandBuffer(VulkanCommandBuffer&&) = default;

    VulkanCommandBuffer(const VulkanDevice&, const vk::CommandPool&);

    void beginRenderPass(const ext::shared_ptr<RenderPass>&, const ext::shared_ptr<Framebuffer>&) override;
    void endRenderPass(void) override;

    const vk::CommandBuffer& vkCommandBuffer(void) const { return m_vkCommandBuffer; }

    ext::optional<vk::ImageLayout> imageSrcLayout(const VulkanTexture&);
    ext::optional<vk::ImageLayout> imageDstLayout(const VulkanTexture&);

    void addImageMemoryBarrier(const VulkanTexture&, const vk::ImageMemoryBarrier&);

    void reset(void);

    ~VulkanCommandBuffer() = default;

private:
    const VulkanDevice* m_device;
    vk::CommandBuffer m_vkCommandBuffer;

    ext::vector<ext::shared_ptr<VulkanRenderPass>> m_usedRenderPasses;
    ext::vector<ext::shared_ptr<VulkanFramebuffer>> m_usedFramebuffers;

    ext::map<VulkanTexture*, ext::pair<vk::ImageLayout, vk::ImageLayout>> m_imageTransitions;

public:
    VulkanCommandBuffer& operator = (const VulkanCommandBuffer&) = delete;
    VulkanCommandBuffer& operator = (VulkanCommandBuffer&&) = default;
};

}

#endif // VULKANCOMMANDBUFFER_HPP
