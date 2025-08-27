/*
 * ---------------------------------------------------
 * VulkanCommandBuffer.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/07 14:34:01
 * ---------------------------------------------------
 */

#include "Graphics/Enums.hpp"
#include "Graphics/Framebuffer.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/ParameterBlock.hpp"

#include "Vulkan/VulkanCommandBuffer.hpp"
#include "Vulkan/Sync.hpp"
#include "Vulkan/VulkanBuffer.hpp"
#include "Vulkan/VulkanParameterBlock.hpp"
#include "Vulkan/VulkanTexture.hpp"
#include "Vulkan/VulkanEnums.hpp"
#include "Vulkan/QueueFamily.hpp"
#include "Vulkan/imgui_impl_vulkan.h"
#include "Vulkan/VulkanGraphicsPipeline.hpp"

namespace gfx
{

VulkanCommandBuffer::VulkanCommandBuffer(vk::CommandBuffer&& commandBuffer, const QueueFamily& queueFamily)
    : m_vkCommandBuffer(std::move(commandBuffer)), m_queueFamily(queueFamily)
{
}

void VulkanCommandBuffer::beginRenderPass(const Framebuffer& framebuffer)
{
    ext::vector<vk::RenderingAttachmentInfo> colorAttachmentInfos(framebuffer.colorAttachments.size());
    ext::optional<vk::RenderingAttachmentInfo> depthAttachmentInfo;
    ext::vector<vk::ImageMemoryBarrier2> imageMemoryBarriers;

    for (size_t i = 0; auto& colorAttachment : framebuffer.colorAttachments)
    {
        ext::shared_ptr<VulkanTexture> texture = dynamic_pointer_cast<VulkanTexture>(colorAttachment.texture);
        assert(texture);

        colorAttachmentInfos[i] = vk::RenderingAttachmentInfo{}
            .setLoadOp(toVkAttachmentLoadOp(colorAttachment.loadAction))
            .setClearValue(vk::ClearValue{}.setColor(vk::ClearColorValue{}.setFloat32({
                  colorAttachment.clearColor[0],
                  colorAttachment.clearColor[1],
                  colorAttachment.clearColor[2],
                  colorAttachment.clearColor[3]})))
            .setImageView(texture->vkImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal);

        ImageSyncRequest syncReq{};
        syncReq.stageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
        syncReq.accessMask = vk::AccessFlagBits2::eColorAttachmentWrite | vk::AccessFlagBits2::eColorAttachmentRead;
        syncReq.layout = vk::ImageLayout::eColorAttachmentOptimal;
        syncReq.preserveContent = colorAttachment.loadAction == LoadAction::load;

        auto it = m_imageFinalSyncStates.find(texture);
        if (it != m_imageFinalSyncStates.end()) {
            auto barrier = syncImage(it->second, syncReq);
            if (barrier.has_value()) {
                barrier->setImage(texture->vkImage());
                barrier->setSubresourceRange(texture->subresourceRange());
                imageMemoryBarriers.push_back(barrier.value());
            }
        } else {
            m_imageSyncRequests[texture] = syncReq;
            m_imageFinalSyncStates[texture] = imageStateAfterSync(syncReq);
        }
    }

    if (auto& depthAttachment = framebuffer.depthAttachment)
    {
        ext::shared_ptr<VulkanTexture> texture = dynamic_pointer_cast<VulkanTexture>(depthAttachment->texture);
        assert(texture);

        depthAttachmentInfo = vk::RenderingAttachmentInfo{}
            .setLoadOp(toVkAttachmentLoadOp(depthAttachment->loadAction))
            .setClearValue(vk::ClearValue{}.setDepthStencil(vk::ClearDepthStencilValue{}.setDepth(depthAttachment->clearDepth)))
            .setImageView(texture->vkImageView())
            .setImageLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

        ImageSyncRequest syncReq{};
        syncReq.stageMask = vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests;
        syncReq.accessMask = vk::AccessFlagBits2::eDepthStencilAttachmentWrite | vk::AccessFlagBits2::eDepthStencilAttachmentRead;
        syncReq.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        syncReq.preserveContent = depthAttachment->loadAction == LoadAction::load;

        auto it = m_imageFinalSyncStates.find(texture);
        if (it != m_imageFinalSyncStates.end()) {
            auto barrier = syncImage(it->second, syncReq);
            if (barrier.has_value()) {
                barrier->setImage(texture->vkImage());
                barrier->setSubresourceRange(texture->subresourceRange());
                imageMemoryBarriers.push_back(barrier.value());
            }
        } else {
            m_imageSyncRequests[texture] = syncReq;
            m_imageFinalSyncStates[texture] = imageStateAfterSync(syncReq);
        }
    }

    if (imageMemoryBarriers.empty() == false) {
        m_vkCommandBuffer.pipelineBarrier2(vk::DependencyInfo{}
            .setDependencyFlags(vk::DependencyFlags{}) // TODO ?
            .setImageMemoryBarriers(imageMemoryBarriers));
    }

    m_viewport = vk::Viewport{}
        .setX(0)
        .setY(0)
        .setWidth(static_cast<float>(framebuffer.colorAttachments[0].texture->width()))
        .setHeight(static_cast<float>(framebuffer.colorAttachments[0].texture->height()))
        .setMinDepth(0)
        .setMaxDepth(1);

    m_scissor = vk::Rect2D{}
        .setOffset({.x=0, .y=0})
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

    m_vkCommandBuffer.beginRendering(renderingInfo);
}

void VulkanCommandBuffer::usePipeline(const ext::shared_ptr<const GraphicsPipeline>& _graphicsPipeline)
{
    auto graphicsPipeline = ext::dynamic_pointer_cast<const VulkanGraphicsPipeline>(_graphicsPipeline);

    m_vkCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline->vkPipeline());
    m_vkCommandBuffer.setViewport(0, m_viewport);
    m_vkCommandBuffer.setScissor(0, m_scissor);

    m_usedPipelines.push_back(graphicsPipeline);
}

void VulkanCommandBuffer::useVertexBuffer(const ext::shared_ptr<Buffer>& aBuffer)
{
    auto buffer = ext::dynamic_pointer_cast<VulkanBuffer>(aBuffer);

    BufferSyncRequest syncReq{};
    syncReq.stageMask = vk::PipelineStageFlagBits2::eVertexInput;
    syncReq.accessMask = vk::AccessFlagBits2::eVertexAttributeRead;

    auto it = m_bufferFinalSyncStates.find(buffer);
    if (it != m_bufferFinalSyncStates.end()) {
        auto barrier = syncBuffer(it->second, syncReq); // will update the final sync state
        if (barrier.has_value()) {
            barrier->setBuffer(buffer->vkBuffer());
            barrier->setOffset(0);
            barrier->setSize(vk::WholeSize);
            m_vkCommandBuffer.pipelineBarrier2(vk::DependencyInfo{}
               .setDependencyFlags(vk::DependencyFlags{})
               .setBufferMemoryBarriers(*barrier));
        }
    } else {
        auto [it1, res1] = m_bufferSyncRequests.insert(ext::make_pair(buffer, syncReq));
        assert(res1);
        auto [it2, res2] = m_bufferFinalSyncStates.insert(ext::make_pair(buffer, bufferStateAfterSync(syncReq)));
        assert(res2);
    }

    m_vkCommandBuffer.bindVertexBuffers(0, buffer->vkBuffer(), {0});
}

void VulkanCommandBuffer::setParameterBlock(const ParameterBlock& aPblock, uint32_t index)
{
    const auto& pBlock = dynamic_cast<const VulkanParameterBlock&>(aPblock);
    ext::vector<vk::BufferMemoryBarrier2> bufferMemoryBarriers;

    for (auto& [binding, buffer] : pBlock.usedBuffers())
    {
        BufferSyncRequest syncReq{};

        if ((binding.usages & BindingUsage::vertexRead) || (binding.usages & BindingUsage::vertexWrite))
            syncReq.stageMask |= vk::PipelineStageFlagBits2::eVertexShader;
        if ((binding.usages & BindingUsage::fragmentRead) || (binding.usages & BindingUsage::fragmentWrite))
            syncReq.stageMask |= vk::PipelineStageFlagBits2::eFragmentShader;

        if (static_cast<bool>(binding.usages & (BindingUsage::vertexRead | BindingUsage::fragmentRead))) {
            assert(binding.type == BindingType::uniformBuffer);
            syncReq.accessMask |= vk::AccessFlagBits2::eUniformRead;
        }
        if (static_cast<bool>(binding.usages & (BindingUsage::vertexWrite | BindingUsage::fragmentWrite))) {
            syncReq.accessMask |= vk::AccessFlagBits2::eShaderWrite;
            throw ext::runtime_error("not implemented");
        }

        auto it = m_bufferFinalSyncStates.find(buffer);
        if (it != m_bufferFinalSyncStates.end()) {
            auto barrier = syncBuffer(it->second, syncReq); // will update the final sync state
            if (barrier.has_value()) {
                barrier->setBuffer(buffer->vkBuffer());
                barrier->setOffset(0);
                barrier->setSize(vk::WholeSize);
                bufferMemoryBarriers.push_back(*barrier);
            }
        } else {
            auto [it1, res1] = m_bufferSyncRequests.insert(ext::make_pair(buffer, syncReq));
            assert(res1);
            auto [it2, res2] = m_bufferFinalSyncStates.insert(ext::make_pair(buffer, bufferStateAfterSync(syncReq)));
            assert(res2);
        }
    }

    if (bufferMemoryBarriers.empty() == false)
    {
        auto dependencyInfo = vk::DependencyInfo{}
            .setDependencyFlags(vk::DependencyFlags{})
            .setBufferMemoryBarriers(bufferMemoryBarriers);

        m_vkCommandBuffer.pipelineBarrier2(dependencyInfo);
    }

    m_vkCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_usedPipelines.back()->pipelineLayout(), index, pBlock.descriptorSet(), {});
}

void VulkanCommandBuffer::drawVertices(uint32_t start, uint32_t count)
{
    m_vkCommandBuffer.draw(count, 1, start, 0);
}

void VulkanCommandBuffer::drawIndexedVertices(const ext::shared_ptr<Buffer>& aBuffer)
{
    auto buffer = ext::dynamic_pointer_cast<VulkanBuffer>(aBuffer);

    BufferSyncRequest syncReq{};
    syncReq.stageMask = vk::PipelineStageFlagBits2::eVertexInput;
    syncReq.accessMask = vk::AccessFlagBits2::eIndexRead;

    auto it = m_bufferFinalSyncStates.find(buffer);
    if (it != m_bufferFinalSyncStates.end()) {
        auto barrier = syncBuffer(it->second, syncReq); // will update the final sync state
        if (barrier.has_value()) {
            barrier->setBuffer(buffer->vkBuffer());
            barrier->setOffset(0);
            barrier->setSize(vk::WholeSize);
            m_vkCommandBuffer.pipelineBarrier2(vk::DependencyInfo{}
               .setDependencyFlags(vk::DependencyFlags{})
               .setBufferMemoryBarriers(*barrier));
        }
    } else {
        auto [it1, res1] = m_bufferSyncRequests.insert(ext::make_pair(buffer, syncReq));
        assert(res1);
        auto [it2, res2] = m_bufferFinalSyncStates.insert(ext::make_pair(buffer, bufferStateAfterSync(syncReq)));
        assert(res2);
    }

    m_vkCommandBuffer.bindIndexBuffer(buffer->vkBuffer(), 0, vk::IndexType::eUint32);
    m_vkCommandBuffer.drawIndexed(static_cast<uint32_t>(buffer->size() / sizeof(uint32_t)), 1, 0, 0, 0);
}

#if defined(GFX_IMGUI_ENABLED)
void VulkanCommandBuffer::imGuiRenderDrawData(ImDrawData* drawData) const
{
    ImGui_ImplVulkan_RenderDrawData(drawData, m_vkCommandBuffer);
}
#endif

void VulkanCommandBuffer::endRenderPass()
{
    m_vkCommandBuffer.endRendering();
}

void VulkanCommandBuffer::clear()
{
    m_signalSemaphore = nullptr;
    m_waitSemaphores.clear();
    m_bufferFinalSyncStates.clear();
    m_bufferSyncRequests.clear();
    m_imageFinalSyncStates.clear();
    m_imageSyncRequests.clear();
    m_usedPipelines.clear();
}

}
