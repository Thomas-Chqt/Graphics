/*
 * ---------------------------------------------------
 * VulkanFramebuffer.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/06 08:18:05
 * ---------------------------------------------------
 */

#define VULKAN_HPP_NO_CONSTRUCTORS

#include "Graphics/Framebuffer.hpp"

#include "Vulkan/VulkanFramebuffer.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanTexture.hpp"
#include "Vulkan/VulkanEnums.hpp"
#include "Vulkan/VulkanRenderPass.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <cassert>
    #include <cstdint>
    #include <memory>
    namespace ext = std;
#endif

namespace gfx
{

VulkanFramebuffer::VulkanFramebuffer(const VulkanDevice& device, const Framebuffer::Descriptor& desc)
    : m_device(&device)
{
    m_attachments.resize(desc.colorAttachments.size() + (desc.depthAttachment ? 1 : 0));
    m_imageViews.resize(m_attachments.size());

    uint32_t i = 0;
    for (auto& attachment : desc.colorAttachments) {
        m_attachments[i] = ext::dynamic_pointer_cast<VulkanTexture>(attachment);
        assert(m_attachments[i]);
        i++;
    }
    if (desc.depthAttachment) {
        m_attachments[i] = ext::dynamic_pointer_cast<VulkanTexture>(desc.depthAttachment);
        assert(m_attachments[i]);
        i++;
    }

    for (size_t i = 0; auto& attachment : m_attachments)
    {
        vk::ImageViewCreateInfo imageCreateInfo {
            .image = attachment->vkImage(),
            .viewType = vk::ImageViewType::e2D,
            .format = toVkFormat(attachment->pixelFormat()),
            .components = {
                .r = vk::ComponentSwizzle::eIdentity,
                .g = vk::ComponentSwizzle::eIdentity,
                .b = vk::ComponentSwizzle::eIdentity,
                .a = vk::ComponentSwizzle::eIdentity},
            .subresourceRange = {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0, .levelCount = 1,
                .baseArrayLayer = 0, .layerCount = 1}
        };

        m_imageViews[i++] = m_device->vkDevice().createImageView(imageCreateInfo);
    }

    const VulkanRenderPass* vulkanRenderPass = dynamic_cast<const VulkanRenderPass*>(desc.renderPass);
    assert(vulkanRenderPass);

    vk::FramebufferCreateInfo framebufferCreateInfo = {
        .renderPass = vulkanRenderPass->vkRenderPass(),
        .width = desc.width, .height = desc.height,
        .layers = 1
    };
    framebufferCreateInfo.setAttachments(m_imageViews);

    m_vkFramebuffer = m_device->vkDevice().createFramebuffer(framebufferCreateInfo);
}

VulkanFramebuffer::~VulkanFramebuffer()
{
    m_device->vkDevice().destroyFramebuffer(m_vkFramebuffer);
    for (auto& view : m_imageViews)
        m_device->vkDevice().destroyImageView(view);
}

}
