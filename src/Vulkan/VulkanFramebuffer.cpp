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
    m_colorAttachments.resize(desc.colorAttachments.size());
    m_imageViews.resize(desc.colorAttachments.size() + (desc.depthAttachment ? 1 : 0));

    for (uint32_t i = 0; auto& attachment : desc.colorAttachments) {
        m_colorAttachments[i] = ext::dynamic_pointer_cast<VulkanTexture>(attachment);
        assert(m_colorAttachments[i]);
        i++;
    }
    if (desc.depthAttachment) {
        m_depthAttachment = ext::dynamic_pointer_cast<VulkanTexture>(desc.depthAttachment);
        assert(m_depthAttachment);
    }

    size_t i = 0;
    for (auto& attachment : m_colorAttachments)
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
    if (auto* depthAttachment = m_depthAttachment.get())
    {
        vk::ImageViewCreateInfo imageCreateInfo {
            .image = depthAttachment->vkImage(),
            .viewType = vk::ImageViewType::e2D,
            .format = toVkFormat(depthAttachment->pixelFormat()),
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

ext::vector<ext::shared_ptr<Texture>> VulkanFramebuffer::colorAttachments(void)
{
    ext::vector<ext::shared_ptr<Texture>> attachments(m_colorAttachments.size());
    for (size_t i = 0; auto& attachment : m_colorAttachments)
        attachments[i] = attachment;
    return attachments;
};

const ext::vector<ext::shared_ptr<Texture>> VulkanFramebuffer::colorAttachments(void) const 
{
    ext::vector<ext::shared_ptr<Texture>> attachments(m_colorAttachments.size());
    for (size_t i = 0; auto& attachment : m_colorAttachments)
        attachments[i] = attachment;
    return attachments;
};

VulkanFramebuffer::~VulkanFramebuffer()
{
    m_device->vkDevice().destroyFramebuffer(m_vkFramebuffer);
    for (auto& view : m_imageViews)
        m_device->vkDevice().destroyImageView(view);
}

}
