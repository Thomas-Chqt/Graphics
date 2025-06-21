/*
 * ---------------------------------------------------
 * VulkanCommandBuffer.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/07 14:34:01
 * ---------------------------------------------------
 */

#define VULKAN_HPP_NO_CONSTRUCTORS

#include "Graphics/RenderPass.hpp"
#include "Graphics/Framebuffer.hpp"

#include "Vulkan/VulkanCommandBuffer.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanRenderPass.hpp"
#include "Vulkan/VulkanFramebuffer.hpp"
#include "Vulkan/VulkanSwapchain.hpp"
#include "Vulkan/VulkanTexture.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    #include <array>
    #include <memory>
    #include <optional>
    namespace ext = std;
#endif

namespace gfx
{

VulkanCommandBuffer::VulkanCommandBuffer(const VulkanDevice& device, const vk::CommandPool& pool)
    : m_device(&device)
{
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo = {
        .commandPool = pool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1
    };
    m_vkCommandBuffer = device.vkDevice().allocateCommandBuffers(commandBufferAllocateInfo).front();
}

void VulkanCommandBuffer::beginRenderPass(const ext::shared_ptr<RenderPass>& _renderPass, const ext::shared_ptr<Framebuffer>& _framebuffer)
{
    const ext::shared_ptr<VulkanRenderPass> renderPass = ext::dynamic_pointer_cast<VulkanRenderPass>(_renderPass);
    const ext::shared_ptr<VulkanFramebuffer> framebuffer = ext::dynamic_pointer_cast<VulkanFramebuffer>(_framebuffer);

    ext::vector<vk::ClearValue> clearValues;
    clearValues.reserve(renderPass->colorAttachmentsDesc().size() + (renderPass->depthAttachmentDesc().has_value() ? 1 : 0));
    for (auto& colorAttachmentDesc : renderPass->colorAttachmentsDesc())
    {
        vk::ClearValue clearColor = {
            .color.float32 = ext::array<float, 4>{
                colorAttachmentDesc.clearColor[0],
                colorAttachmentDesc.clearColor[1],
                colorAttachmentDesc.clearColor[2],
                colorAttachmentDesc.clearColor[3]
            }
        };
        clearValues.push_back(clearColor);
    }
    if (renderPass->depthAttachmentDesc().has_value())
    {
        auto& depthAttachmentDesc = renderPass->depthAttachmentDesc().value();
        vk::ClearValue clearDepth = {
            .depthStencil.depth = depthAttachmentDesc.clearDepth
        };
        clearValues.push_back(clearDepth);
    }

    vk::RenderPassBeginInfo renderPassBeginInfo = {
        .renderPass = renderPass->vkRenderPass(),
        .framebuffer = framebuffer->vkFramebuffer(),
        .renderArea = {
            .offset = {0, 0},
            .extent = {
                .width = framebuffer->colorAttachment(0)->width(),
                .height = framebuffer->colorAttachment(0)->height(),
            },
        },
    };
    renderPassBeginInfo.setClearValues(clearValues);

    m_usedRenderPasses.push_back(renderPass);

    m_usedFramebuffers.push_back(framebuffer);
    for (auto& colorAttachment : framebuffer->vkColorAttachments()) {
        m_imageTransitions[colorAttachment.get()].first = vk::ImageLayout::eColorAttachmentOptimal;
        m_imageTransitions[colorAttachment.get()].second = vk::ImageLayout::eGeneral;
    }
    if (auto depthAttachement = framebuffer->vkDepthAttachment()) {
        m_imageTransitions[depthAttachement.get()].first = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        m_imageTransitions[depthAttachement.get()].second = vk::ImageLayout::eGeneral;
    }

    m_vkCommandBuffer.begin(vk::CommandBufferBeginInfo{});
    m_vkCommandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
}

void VulkanCommandBuffer::endRenderPass(void)
{
    m_vkCommandBuffer.endRenderPass();
}

ext::optional<vk::ImageLayout> VulkanCommandBuffer::imageSrcLayout(const VulkanTexture& texture)
{
    auto it = m_imageTransitions.find((VulkanTexture*)&texture);
    if (it == m_imageTransitions.end())
        return ext::nullopt;
    return it->second.first;
}

ext::optional<vk::ImageLayout> VulkanCommandBuffer::imageDstLayout(const VulkanTexture& texture)
{
    auto it = m_imageTransitions.find((VulkanTexture*)&texture);
    if (it == m_imageTransitions.end())
        return ext::nullopt;
    return it->second.second;
}

void VulkanCommandBuffer::addImageMemoryBarrier(const VulkanTexture& texture, const vk::ImageMemoryBarrier& _barrier)
{
    auto it = m_imageTransitions.find((VulkanTexture*)&texture);
    if (it == m_imageTransitions.end())
        return;
    
    vk::ImageMemoryBarrier barrier = _barrier;
    barrier.setOldLayout(it->second.second);
    barrier.setImage(texture.vkImage());

    it->second.second = barrier.newLayout;

    m_vkCommandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::PipelineStageFlagBits::eTransfer,
        {},
        nullptr,
        nullptr,
        barrier);
}

void VulkanCommandBuffer::reset(void)
{
    m_imageTransitions.clear();
    m_usedFramebuffers.clear();
    m_usedRenderPasses.clear();
}

}
