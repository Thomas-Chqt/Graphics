/*
 * ---------------------------------------------------
 * VulkanCommandBuffer.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/07 14:34:01
 * ---------------------------------------------------
 */

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_NO_CONSTRUCTORS

#include "Graphics/Framebuffer.hpp"

#include "Vulkan/VulkanCommandBuffer.hpp"
#include "Vulkan/VulkanTexture.hpp"
#include "Vulkan/VulkanEnums.hpp"
#include "Vulkan/QueueFamily.hpp"
#include "VulkanGraphicsPipeline.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    #include <array>
    #include <memory>
    #include <optional>
    #include <utility>
    #include <cstddef>
    namespace ext = std;
#endif

namespace gfx
{

VulkanCommandBuffer::VulkanCommandBuffer(vk::CommandBuffer&& commandBuffer, const QueueFamily& queueFamily, bool useDynamicRenderingExt)
    : m_vkCommandBuffer(std::move(commandBuffer)), m_queueFamily(queueFamily), m_useDynamicRenderingExt(useDynamicRenderingExt)
{
}

void VulkanCommandBuffer::beginRenderPass(const Framebuffer& framebuffer)
{
    ext::vector<vk::RenderingAttachmentInfo> colorAttachmentInfos(framebuffer.colorAttachments.size());
    ext::optional<vk::RenderingAttachmentInfo> depthAttachmentInfo;
    ext::vector<vk::ImageMemoryBarrier> imageMemoryBarriers;

    for (size_t i = 0; auto& colorAttachment : framebuffer.colorAttachments)
    {
        const VulkanTexture& vkTexture = dynamic_cast<const VulkanTexture&>(*colorAttachment.texture);
        colorAttachmentInfos[i] = vk::RenderingAttachmentInfo{}
            .setLoadOp(toVkAttachmentLoadOp(colorAttachment.loadAction))
            .setClearValue(vk::ClearValue{
                .color{
                    .float32 = ext::array<float, 4>{
                        colorAttachment.clearColor[0],
                        colorAttachment.clearColor[1],
                        colorAttachment.clearColor[2],
                        colorAttachment.clearColor[3]
                    }
                }
            })
            .setImageView(vkTexture.vkImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal);

        auto imageMemoryBarrier = vk::ImageMemoryBarrier{}
            .setSrcAccessMask(vk::AccessFlagBits{})
            .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
            .setOldLayout(vk::ImageLayout::eUndefined)
            .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setSrcQueueFamilyIndex(m_queueFamily.index)
            .setDstQueueFamilyIndex(m_queueFamily.index)
            .setImage(vkTexture.vkImage())
            .setSubresourceRange({
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            });
        imageMemoryBarriers.push_back(imageMemoryBarrier);
    }

    if (auto& depthAttachment = framebuffer.depthAttachment)
    {
        const VulkanTexture& vkTexture = dynamic_cast<const VulkanTexture&>(*depthAttachment->texture);
        depthAttachmentInfo = vk::RenderingAttachmentInfo{}
            .setLoadOp(toVkAttachmentLoadOp(depthAttachment->loadAction))
            .setClearValue({
                .depthStencil{
                    .depth = depthAttachment->clearDepth
                }
            })
            .setImageView(vkTexture.vkImageView())
            .setImageLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

        auto imageMemoryBarrier = vk::ImageMemoryBarrier{}
            .setSrcAccessMask(vk::AccessFlagBits{})
            .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
            .setOldLayout(vk::ImageLayout::eUndefined)
            .setNewLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
            .setSrcQueueFamilyIndex(m_queueFamily.index)
            .setDstQueueFamilyIndex(m_queueFamily.index)
            .setImage(vkTexture.vkImage())
            .setSubresourceRange({
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            });
        imageMemoryBarriers.push_back(imageMemoryBarrier);

    }

    m_viewport = vk::Viewport{}
        .setX(0)
        .setY(0)
        .setWidth(framebuffer.colorAttachments[0].texture->width())
        .setHeight(framebuffer.colorAttachments[0].texture->height())
        .setMinDepth(0)
        .setMaxDepth(1);

    m_scissor = vk::Rect2D{}
        .setOffset({0, 0})
        .setExtent({
            .width = framebuffer.colorAttachments[0].texture->width(),
            .height = framebuffer.colorAttachments[0].texture->height()
        });

    auto renderingInfo = vk::RenderingInfo{}
        .setRenderArea(m_scissor)
        .setLayerCount(1)
        .setViewMask(0)
        .setColorAttachments(colorAttachmentInfos)
        .setPDepthAttachment(depthAttachmentInfo ? &depthAttachmentInfo.value() : nullptr);

    if (ext::exchange(m_isBegin, true) == false)
        m_vkCommandBuffer.begin(vk::CommandBufferBeginInfo{});

    m_vkCommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eTransfer, {},
                                      nullptr, nullptr, imageMemoryBarriers);
    
    if (m_useDynamicRenderingExt)
        m_vkCommandBuffer.beginRenderingKHR(renderingInfo);
    else
        m_vkCommandBuffer.beginRendering(renderingInfo);
}

void VulkanCommandBuffer::usePipeline(const ext::shared_ptr<GraphicsPipeline>& _graphicsPipeline)
{
    ext::shared_ptr<VulkanGraphicsPipeline> graphicsPipeline = ext::dynamic_pointer_cast<VulkanGraphicsPipeline>(_graphicsPipeline);
    
    m_vkCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline->vkPipeline());
    m_vkCommandBuffer.setViewport(0, m_viewport);
    m_vkCommandBuffer.setScissor(0, m_scissor);
}

void VulkanCommandBuffer::drawVertices(uint32_t start, uint32_t count)
{
    m_vkCommandBuffer.draw(count, 1, start, 0);
}

void VulkanCommandBuffer::endRenderPass(void)
{
    if (m_useDynamicRenderingExt)
        m_vkCommandBuffer.endRenderingKHR();
    else
        m_vkCommandBuffer.endRendering();
}

}
