/*
 * ---------------------------------------------------
 * VulkanCommandBuffer.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/07 14:34:01
 * ---------------------------------------------------
 */

#include "Graphics/Enums.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/ParameterBlock.hpp"

#include "Vulkan/VulkanCommandBuffer.hpp"
#include "Vulkan/Sync.hpp"
#include "Vulkan/VulkanBuffer.hpp"
#include "Vulkan/VulkanDrawable.hpp"
#include "Vulkan/VulkanParameterBlock.hpp"
#include "Vulkan/VulkanSampler.hpp"
#include "Vulkan/VulkanTexture.hpp"
#include "Vulkan/VulkanEnums.hpp"
#include "Vulkan/VulkanPassDescriptor.hpp"
#include "Vulkan/VulkanGraphicsPipeline.hpp"
#include "Vulkan/VulkanComputePipeline.hpp"
#include "Vulkan/VulkanCommandBufferPool.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"
#include "Vulkan/VulkanTextureView.hpp"

#define m_usedPipelines m_nonReusedRessources.usedPipelines
#define m_boundPipeline m_nonReusedRessources.boundPipeline
#define m_usedPBlock m_nonReusedRessources.usedPBlock
#define m_imageSyncRequests m_nonReusedRessources.imageSyncRequests
#define m_imageSyncStates m_nonReusedRessources.imageSyncStates
#define m_bufferSyncRequests m_nonReusedRessources.bufferSyncRequests
#define m_bufferFinalSyncStates m_nonReusedRessources.bufferFinalSyncStates
#define m_presentedDrawables m_nonReusedRessources.presentedDrawables

namespace gfx
{

namespace
{
    vk::ClearValue toVkColorClearValue(const ClearValue& clearValue)
    {
        return std::visit([]<typename T>(const T& clear) -> vk::ClearValue {
            if constexpr (std::is_same_v<T, ClearFloatColor>)
                return vk::ClearValue{}.setColor(vk::ClearColorValue{}.setFloat32(clear.value));
            else if constexpr (std::is_same_v<T, ClearUIntColor>)
                return vk::ClearValue{}.setColor(vk::ClearColorValue{}.setUint32(clear.value));
            else
                std::unreachable();
        }, clearValue.value);
    }

    vk::ClearValue toVkDepthClearValue(const ClearValue& clearValue)
    {
        const auto* clearDepth = std::get_if<ClearDepth>(&clearValue.value);
        assert(clearDepth);
        return vk::ClearValue{}.setDepthStencil(vk::ClearDepthStencilValue{}.setDepth(clearDepth->value));
    }
}

VulkanCommandBuffer::VulkanCommandBuffer(const VulkanDevice* device, const std::shared_ptr<vk::CommandPool>& commandPool)
    : m_device(device),
      m_vkCommandPool(commandPool)
{
    assert(m_device);
    assert(m_vkCommandPool);

    auto commandBufferAllocateInfo = vk::CommandBufferAllocateInfo{}
        .setCommandPool(*m_vkCommandPool)
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandBufferCount(1);
    m_vkCommandBuffer = m_device->vkDevice().allocateCommandBuffers(commandBufferAllocateInfo).front();
}

VulkanCommandBuffer::VulkanCommandBuffer(const VulkanDevice* device, const vk::CommandPool& commandPool)
    : m_device(device)
{
    assert(m_device);

    auto commandBufferAllocateInfo = vk::CommandBufferAllocateInfo{}
        .setCommandPool(commandPool)
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandBufferCount(1);
    m_vkCommandBuffer = m_device->vkDevice().allocateCommandBuffers(commandBufferAllocateInfo).front();
}

void VulkanCommandBuffer::beginRenderPass(RenderPassDescriptor& descriptor)
{
    assert(m_boundPipeline == nullptr);

    auto* vulkanDescriptor = dynamic_cast<VulkanRenderPassDescriptor*>(&descriptor);
    assert(vulkanDescriptor);
    vulkanDescriptor->invokeBeginCallback(m_vkCommandBuffer);

    const auto& colorAttachments = descriptor.colorAttachments();
    const auto& depthAttachment = descriptor.depthAttachment();
    assert(colorAttachments.empty() == false);

    std::vector<vk::RenderingAttachmentInfo> colorAttachmentInfos(colorAttachments.size());
    std::optional<vk::RenderingAttachmentInfo> depthAttachmentInfo;
    std::vector<vk::ImageMemoryBarrier2> imageMemoryBarriers;

    for (size_t i = 0; const auto& colorAttachment : colorAttachments)
    {
        std::shared_ptr<VulkanTextureView> textureView = dynamic_pointer_cast<VulkanTextureView>(colorAttachment.texture);
        assert(textureView);
        assert(textureView->mipLevelCount() == 1);
        assert(textureView->subresourceRange().layerCount == 1);

        colorAttachmentInfos[i] = vk::RenderingAttachmentInfo{}
            .setLoadOp(toVkAttachmentLoadOp(colorAttachment.loadAction))
            .setClearValue(toVkColorClearValue(colorAttachment.clearValue))
            .setImageView(textureView->vkImageView())
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal);

        ImageSyncRequest syncReq{};
        syncReq.stageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
        syncReq.accessMask = vk::AccessFlagBits2::eColorAttachmentWrite | vk::AccessFlagBits2::eColorAttachmentRead;
        syncReq.layout = vk::ImageLayout::eColorAttachmentOptimal;
        syncReq.preserveContent = colorAttachment.loadAction == LoadAction::load;

        auto it = m_imageSyncStates.find(textureView->rootTexture());
        if (it != m_imageSyncStates.end()) {
            auto barrier = syncImage(it->second, syncReq);
            if (barrier.has_value()) {
                barrier->setImage(textureView->rootTexture()->vkImage());
                barrier->setSubresourceRange(textureView->subresourceRange());
                imageMemoryBarriers.push_back(barrier.value());
            }
        } else {
            m_imageSyncRequests[textureView->rootTexture()] = syncReq;
            m_imageSyncStates[textureView->rootTexture()] = imageStateAfterSync(syncReq);
        }

        i++;
    }

    if (depthAttachment)
    {
        std::shared_ptr<VulkanTextureView> texture = dynamic_pointer_cast<VulkanTextureView>(depthAttachment->texture);
        assert(texture);
        assert(texture->mipLevelCount() == 1);
        assert(texture->subresourceRange().layerCount == 1);

        depthAttachmentInfo = vk::RenderingAttachmentInfo{}
            .setLoadOp(toVkAttachmentLoadOp(depthAttachment->loadAction))
            .setClearValue(toVkDepthClearValue(depthAttachment->clearValue))
            .setImageView(texture->vkImageView())
            .setImageLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

        ImageSyncRequest syncReq{};
        syncReq.stageMask = vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests;
        syncReq.accessMask = vk::AccessFlagBits2::eDepthStencilAttachmentWrite | vk::AccessFlagBits2::eDepthStencilAttachmentRead;
        syncReq.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        syncReq.preserveContent = depthAttachment->loadAction == LoadAction::load;

        auto it = m_imageSyncStates.find(texture->rootTexture());
        if (it != m_imageSyncStates.end()) {
            auto barrier = syncImage(it->second, syncReq);
            if (barrier.has_value()) {
                barrier->setImage(texture->rootTexture()->vkImage());
                barrier->setSubresourceRange(texture->subresourceRange());
                imageMemoryBarriers.push_back(barrier.value());
            }
        } else {
            m_imageSyncRequests[texture->rootTexture()] = syncReq;
            m_imageSyncStates[texture->rootTexture()] = imageStateAfterSync(syncReq);
        }
    }

    if (imageMemoryBarriers.empty() == false) {
        m_vkCommandBuffer.pipelineBarrier2(vk::DependencyInfo{}
            .setDependencyFlags(vk::DependencyFlags{}) // TODO ?
            .setImageMemoryBarriers(imageMemoryBarriers));
    }

    m_vkCommandBuffer.setViewport(0, vk::Viewport{}
        .setX(0)
        .setY(0)
        .setWidth(static_cast<float>(colorAttachments[0].texture->width()))
        .setHeight(static_cast<float>(colorAttachments[0].texture->height()))
        .setMinDepth(0)
        .setMaxDepth(1));

    m_vkCommandBuffer.setScissor(0, vk::Rect2D{}
        .setOffset({.x=0, .y=0})
        .setExtent({
            .width = colorAttachments[0].texture->width(),
            .height = colorAttachments[0].texture->height()
        }));

    auto renderingInfo = vk::RenderingInfo{}
        .setRenderArea(vk::Rect2D{}
            .setOffset({.x=0, .y=0})
            .setExtent({
                .width = colorAttachments[0].texture->width(),
                .height = colorAttachments[0].texture->height()
            }))
        .setLayerCount(1)
        .setViewMask(0)
        .setColorAttachments(colorAttachmentInfos)
        .setPDepthAttachment(depthAttachmentInfo ? &depthAttachmentInfo.value() : nullptr);

    m_vkCommandBuffer.beginRendering(renderingInfo);
}

void VulkanCommandBuffer::usePipeline(const std::shared_ptr<const Pipeline>& pipeline)
{
    if (auto graphicsPipeline = std::dynamic_pointer_cast<const VulkanGraphicsPipeline>(pipeline))
        m_vkCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline->vkPipeline());
    else if (auto computePipeline = std::dynamic_pointer_cast<const VulkanComputePipeline>(pipeline))
        m_vkCommandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, computePipeline->vkPipeline());
    else
        std::unreachable();

    m_usedPipelines.insert(pipeline);
    m_boundPipeline = pipeline.get();
}

void VulkanCommandBuffer::useVertexBuffer(const std::shared_ptr<Buffer>& aBuffer)
{
    auto buffer = std::dynamic_pointer_cast<VulkanBuffer>(aBuffer);
    assert(buffer);

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
        m_bufferSyncRequests[buffer] = syncReq;
        m_bufferFinalSyncStates[buffer] = bufferStateAfterSync(syncReq);
    }

    m_vkCommandBuffer.bindVertexBuffers(0, buffer->vkBuffer(), {0});
}

void VulkanCommandBuffer::setParameterBlock(const std::shared_ptr<const ParameterBlock>& aPblock, uint32_t index)
{
    const auto& pBlock = std::dynamic_pointer_cast<const VulkanParameterBlock>(aPblock);
    assert(pBlock);

    assert(m_boundPipeline);

    std::vector<vk::BufferMemoryBarrier2> bufferMemoryBarriers;
    std::vector<vk::ImageMemoryBarrier2> imageMemoryBarriers;

    for (auto& [buffer, binding] : pBlock->usedBuffers())
    {
        BufferSyncRequest syncReq{};

        if ((binding.usages & BindingUsage::vertexRead) || (binding.usages & BindingUsage::vertexWrite))
            syncReq.stageMask |= vk::PipelineStageFlagBits2::eVertexShader;
        if ((binding.usages & BindingUsage::fragmentRead) || (binding.usages & BindingUsage::fragmentWrite))
            syncReq.stageMask |= vk::PipelineStageFlagBits2::eFragmentShader;
        if ((binding.usages & BindingUsage::computeRead) || (binding.usages & BindingUsage::computeWrite))
            syncReq.stageMask |= vk::PipelineStageFlagBits2::eComputeShader;

        if (static_cast<bool>(binding.usages & (BindingUsage::vertexRead | BindingUsage::fragmentRead | BindingUsage::computeRead)))
        {
            switch (binding.type)
            {
                case BindingType::constantBuffer:
                    syncReq.accessMask |= vk::AccessFlagBits2::eUniformRead;
                    break;
                case BindingType::structuredBuffer:
                    syncReq.accessMask |= vk::AccessFlagBits2::eShaderStorageRead;
                    break;
                default:
                    std::unreachable();
            }
        }
        if (static_cast<bool>(binding.usages & (BindingUsage::vertexWrite | BindingUsage::fragmentWrite | BindingUsage::computeWrite))) {
            assert(binding.type == BindingType::structuredBuffer);
            syncReq.accessMask |= vk::AccessFlagBits2::eShaderStorageWrite;
        }

        assert(syncReq.stageMask != vk::PipelineStageFlags2{});
        assert(syncReq.accessMask != vk::AccessFlags2{});

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
            m_bufferSyncRequests[buffer] = syncReq;
            m_bufferFinalSyncStates[buffer] = bufferStateAfterSync(syncReq);
        }
    }

    for (auto& [textureView, binding] : pBlock->usedTextureViews())
    {
        ImageSyncRequest syncReq{};

        if ((binding.usages & BindingUsage::vertexRead) || (binding.usages & BindingUsage::vertexWrite))
            syncReq.stageMask |= vk::PipelineStageFlagBits2::eVertexShader;
        if ((binding.usages & BindingUsage::fragmentRead) || (binding.usages & BindingUsage::fragmentWrite))
            syncReq.stageMask |= vk::PipelineStageFlagBits2::eFragmentShader;
        if ((binding.usages & BindingUsage::computeRead) || (binding.usages & BindingUsage::computeWrite))
            syncReq.stageMask |= vk::PipelineStageFlagBits2::eComputeShader;

        if (static_cast<bool>(binding.usages & (BindingUsage::vertexRead | BindingUsage::fragmentRead | BindingUsage::computeRead))) {
            assert(binding.type == BindingType::sampledTexture || binding.type == BindingType::storageTexture);
            if (binding.type == BindingType::sampledTexture) {
                syncReq.accessMask |= vk::AccessFlagBits2::eShaderRead;
                syncReq.layout = vk::ImageLayout::eShaderReadOnlyOptimal;
            }
            else if (binding.type == BindingType::storageTexture) {
                syncReq.accessMask |= vk::AccessFlagBits2::eShaderStorageRead;
                syncReq.layout = vk::ImageLayout::eGeneral;
            }
            else {
                std::unreachable();
            }
            syncReq.preserveContent = true;
        }
        if (static_cast<bool>(binding.usages & (BindingUsage::vertexWrite | BindingUsage::fragmentWrite | BindingUsage::computeWrite))) {
            assert(binding.type == BindingType::storageTexture);
            syncReq.accessMask |= vk::AccessFlagBits2::eShaderStorageWrite;
            syncReq.layout = vk::ImageLayout::eGeneral;
            syncReq.preserveContent = true;
        }

        assert(syncReq.stageMask != vk::PipelineStageFlags2{});
        assert(syncReq.accessMask != vk::AccessFlags2{});

        auto it = m_imageSyncStates.find(textureView->rootTexture());
        if (it != m_imageSyncStates.end()) {
            auto barrier = syncImage(it->second, syncReq); // will update the final sync state
            if (barrier.has_value()) {
                barrier->setImage(textureView->rootTexture()->vkImage());
                barrier->setSubresourceRange(textureView->subresourceRange());
                imageMemoryBarriers.push_back(*barrier);
            }
        } else {
            m_imageSyncRequests[textureView->rootTexture()] = syncReq;
            m_imageSyncStates[textureView->rootTexture()] = imageStateAfterSync(syncReq);
        }
    }

    if (bufferMemoryBarriers.empty() == false || imageMemoryBarriers.empty() == false)
    {
        auto dependencyInfo = vk::DependencyInfo{}
            .setDependencyFlags(vk::DependencyFlags{});

        if (bufferMemoryBarriers.empty() == false)
            dependencyInfo.setBufferMemoryBarriers(bufferMemoryBarriers);
        if (imageMemoryBarriers.empty() == false)
            dependencyInfo.setImageMemoryBarriers(imageMemoryBarriers);

        m_vkCommandBuffer.pipelineBarrier2(dependencyInfo);
    }

    if (auto* graphicPipeline = dynamic_cast<const VulkanGraphicsPipeline*>(m_boundPipeline))
        m_vkCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, graphicPipeline->pipelineLayout(), index, pBlock->descriptorSet(), {});
    else if (auto* computePipeline = dynamic_cast<const VulkanComputePipeline*>(m_boundPipeline))
        m_vkCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, computePipeline->pipelineLayout(), index, pBlock->descriptorSet(), {});
    else
        std::unreachable();

    m_usedPBlock.insert(pBlock);
}

void VulkanCommandBuffer::setPushConstants(const void* data, size_t size)
{
    assert(size <= 128);
    assert(m_boundPipeline);
    if (auto* graphicPipeline = dynamic_cast<const VulkanGraphicsPipeline*>(m_boundPipeline))
        m_vkCommandBuffer.pushConstants(graphicPipeline->pipelineLayout(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0, static_cast<uint32_t>(size), data);
    else if (auto* computePipeline = dynamic_cast<const VulkanComputePipeline*>(m_boundPipeline))
        m_vkCommandBuffer.pushConstants(computePipeline->pipelineLayout(), vk::ShaderStageFlagBits::eCompute, 0, static_cast<uint32_t>(size), data);
    else
        std::unreachable();
}

void VulkanCommandBuffer::drawVertices(uint32_t start, uint32_t count)
{
    assert(dynamic_cast<const VulkanGraphicsPipeline*>(m_boundPipeline));
    m_vkCommandBuffer.draw(count, 1, start, 0);
}

void VulkanCommandBuffer::drawIndexedVertices(const std::shared_ptr<Buffer>& aBuffer)
{
    auto buffer = std::dynamic_pointer_cast<VulkanBuffer>(aBuffer);
    assert(buffer);

    assert(dynamic_cast<const VulkanGraphicsPipeline*>(m_boundPipeline));

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
        m_bufferSyncRequests[buffer] = syncReq;
        m_bufferFinalSyncStates[buffer] = bufferStateAfterSync(syncReq);
    }

    m_vkCommandBuffer.bindIndexBuffer(buffer->vkBuffer(), 0, vk::IndexType::eUint32);
    m_vkCommandBuffer.drawIndexed(static_cast<uint32_t>(buffer->size() / sizeof(uint32_t)), 1, 0, 0, 0);
}

void VulkanCommandBuffer::endRenderPass()
{
    m_vkCommandBuffer.endRendering();
    m_boundPipeline = nullptr;
}

void VulkanCommandBuffer::beginComputePass(ComputePassDescriptor& descriptor)
{
    assert(m_boundPipeline == nullptr);

    auto* vulkanDescriptor = dynamic_cast<VulkanComputePassDescriptor*>(&descriptor);
    assert(vulkanDescriptor);

    vulkanDescriptor->invokeBeginCallback(m_vkCommandBuffer);
}

void VulkanCommandBuffer::dispatchThreadgroups(uint32_t x, uint32_t y, uint32_t z)
{
    assert(x > 0 && y > 0 && z > 0);
    assert(dynamic_cast<const VulkanComputePipeline*>(m_boundPipeline));
    assert(x <= m_device->physicalDevice().getProperties().limits.maxComputeWorkGroupCount[0]);
    assert(y <= m_device->physicalDevice().getProperties().limits.maxComputeWorkGroupCount[1]);
    assert(z <= m_device->physicalDevice().getProperties().limits.maxComputeWorkGroupCount[2]);

    m_vkCommandBuffer.dispatch(x, y, z);
}

void VulkanCommandBuffer::endComputePass()
{
    m_boundPipeline = nullptr;
}

void VulkanCommandBuffer::beginBlitPass(BlitPassDescriptor& descriptor)
{
    auto* vulkanDescriptor = dynamic_cast<VulkanBlitPassDescriptor*>(&descriptor);
    assert(vulkanDescriptor);

    assert(m_boundPipeline == nullptr);

    vulkanDescriptor->invokeBeginCallback(m_vkCommandBuffer);
}

void VulkanCommandBuffer::copyBufferToBuffer(const std::shared_ptr<Buffer>& aSrc, const std::shared_ptr<Buffer>& aDst, size_t size)
{
    auto src = std::dynamic_pointer_cast<VulkanBuffer>(aSrc);
    assert(src);
    auto dst = std::dynamic_pointer_cast<VulkanBuffer>(aDst);
    assert(dst);

    assert(src->usages() & BufferUsage::copySource);
    assert(dst->usages() & BufferUsage::copyDestination);
    assert(size <= src->size());
    assert(size <= dst->size());

    std::vector<vk::BufferMemoryBarrier2> bufferMemoryBarriers;

    BufferSyncRequest srcBufferSyncReq{};
    srcBufferSyncReq.stageMask = vk::PipelineStageFlagBits2::eTransfer;
    srcBufferSyncReq.accessMask = vk::AccessFlagBits2::eTransferRead;

    auto srcBufferIt = m_bufferFinalSyncStates.find(src);
    if (srcBufferIt != m_bufferFinalSyncStates.end()) {
        auto barrier = syncBuffer(srcBufferIt->second, srcBufferSyncReq); // will update the final sync state
        if (barrier.has_value()) {
            barrier->setBuffer(src->vkBuffer());
            barrier->setOffset(0);
            barrier->setSize(vk::WholeSize);
            bufferMemoryBarriers.push_back(*barrier);
        }
    } else {
        m_bufferSyncRequests[src] = srcBufferSyncReq;
        m_bufferFinalSyncStates[src] = bufferStateAfterSync(srcBufferSyncReq);
    }

    BufferSyncRequest dstBufferSyncReq{};
    dstBufferSyncReq.stageMask = vk::PipelineStageFlagBits2::eTransfer;
    dstBufferSyncReq.accessMask = vk::AccessFlagBits2::eTransferWrite;

    auto dstBufferIt = m_bufferFinalSyncStates.find(dst);
    if (dstBufferIt != m_bufferFinalSyncStates.end()) {
        auto barrier = syncBuffer(dstBufferIt->second, dstBufferSyncReq); // will update the final sync state
        if (barrier.has_value()) {
            barrier->setBuffer(dst->vkBuffer());
            barrier->setOffset(0);
            barrier->setSize(vk::WholeSize);
            bufferMemoryBarriers.push_back(*barrier);
        }
    } else {
        m_bufferSyncRequests[dst] = dstBufferSyncReq;
        m_bufferFinalSyncStates[dst] = bufferStateAfterSync(dstBufferSyncReq);
    }

    if (bufferMemoryBarriers.empty() == false)
    {
        m_vkCommandBuffer.pipelineBarrier2(vk::DependencyInfo{}
            .setDependencyFlags(vk::DependencyFlags{})
            .setBufferMemoryBarriers(bufferMemoryBarriers));
    }

    auto bufferCopy = vk::BufferCopy{}
        .setSrcOffset(0)
        .setDstOffset(0)
        .setSize(size);

    m_vkCommandBuffer.copyBuffer(src->vkBuffer(), dst->vkBuffer(), bufferCopy);
}

void VulkanCommandBuffer::copyBufferToTexture(const std::shared_ptr<Buffer>& aBuffer, size_t bufferOffset, const std::shared_ptr<Texture>& aTexture, uint32_t layerIndex)
{
    auto buffer = std::dynamic_pointer_cast<VulkanBuffer>(aBuffer);
    assert(buffer);
    auto textureView = std::dynamic_pointer_cast<VulkanTextureView>(aTexture);
    assert(textureView);

    assert(buffer->usages() & BufferUsage::copySource);
    assert(textureView->usages() & TextureUsage::copyDestination);
    assert(layerIndex < (textureView->type() == TextureType::textureCube ? 6u : textureView->arrayLayerCount()));
    assert(bufferOffset + pixelFormatSize(textureView->pixelFormat()) * textureView->width() * textureView->height() <= buffer->size());

    std::vector<vk::BufferMemoryBarrier2> bufferMemoryBarriers;
    std::vector<vk::ImageMemoryBarrier2> imageMemoryBarriers;

    BufferSyncRequest bufferSyncReq{};
    bufferSyncReq.stageMask = vk::PipelineStageFlagBits2::eTransfer;
    bufferSyncReq.accessMask = vk::AccessFlagBits2::eTransferRead;

    auto it1 = m_bufferFinalSyncStates.find(buffer);
    if (it1 != m_bufferFinalSyncStates.end()) {
        auto barrier = syncBuffer(it1->second, bufferSyncReq); // will update the final sync state
        if (barrier.has_value()) {
            barrier->setBuffer(buffer->vkBuffer());
            barrier->setOffset(0);
            barrier->setSize(vk::WholeSize);
            bufferMemoryBarriers.push_back(*barrier);
        }
    } else {
        m_bufferSyncRequests[buffer] = bufferSyncReq;
        m_bufferFinalSyncStates[buffer] = bufferStateAfterSync(bufferSyncReq);
    }

    ImageSyncRequest imageSyncReq{};
    imageSyncReq.stageMask = vk::PipelineStageFlagBits2::eTransfer;
    imageSyncReq.accessMask = vk::AccessFlagBits2::eTransferWrite;
    imageSyncReq.layout = vk::ImageLayout::eTransferDstOptimal;
    imageSyncReq.preserveContent = false;

    auto it2 = m_imageSyncStates.find(textureView->rootTexture());
    if (it2 != m_imageSyncStates.end()) {
        auto barrier = syncImage(it2->second, imageSyncReq); // will update the final sync state
        if (barrier.has_value()) {
            barrier->setImage(textureView->rootTexture()->vkImage());
            barrier->setSubresourceRange(textureView->subresourceRange());
            imageMemoryBarriers.push_back(*barrier);
        }
    } else {
        m_imageSyncRequests[textureView->rootTexture()] = imageSyncReq;
        m_imageSyncStates[textureView->rootTexture()] = imageStateAfterSync(imageSyncReq);
    }

    if (bufferMemoryBarriers.empty() == false || imageMemoryBarriers.empty() == false)
    {
        auto dependencyInfo = vk::DependencyInfo{}
            .setDependencyFlags(vk::DependencyFlags{});

        if (bufferMemoryBarriers.empty() == false)
            dependencyInfo.setBufferMemoryBarriers(bufferMemoryBarriers);
        if (imageMemoryBarriers.empty() == false)
            dependencyInfo.setImageMemoryBarriers(imageMemoryBarriers);

        m_vkCommandBuffer.pipelineBarrier2(dependencyInfo);
    }

    auto bufferImageCopy = vk::BufferImageCopy{}
        .setBufferOffset(bufferOffset)
        .setImageSubresource(vk::ImageSubresourceLayers{}
            .setAspectMask(textureView->subresourceRange().aspectMask)
            .setMipLevel(textureView->baseMipLevel())
            .setBaseArrayLayer(layerIndex)
            .setLayerCount(1))
        .setImageExtent(vk::Extent3D{}
            .setWidth(textureView->width())
            .setHeight(textureView->height())
            .setDepth(1));

    m_vkCommandBuffer.copyBufferToImage(
        buffer->vkBuffer(),
        textureView->rootTexture()->vkImage(),
        vk::ImageLayout::eTransferDstOptimal,
        bufferImageCopy);
}

void VulkanCommandBuffer::copyTextureToBuffer(const std::shared_ptr<Texture>& aTexture, uint32_t layerIndex, const std::shared_ptr<Buffer>& aBuffer, size_t bufferOffset)
{
    auto textureView = std::dynamic_pointer_cast<VulkanTextureView>(aTexture);
    assert(textureView);
    auto buffer = std::dynamic_pointer_cast<VulkanBuffer>(aBuffer);
    assert(buffer);

    assert(textureView->usages() & TextureUsage::copySource);
    assert(layerIndex < textureView->arrayLayerCount());
    assert(buffer->usages() & BufferUsage::copyDestination);
    assert(bufferOffset + pixelFormatSize(textureView->pixelFormat()) * textureView->width() * textureView->height() <= buffer->size());

    std::vector<vk::ImageMemoryBarrier2> imageMemoryBarriers;
    std::vector<vk::BufferMemoryBarrier2> bufferMemoryBarriers;

    ImageSyncRequest imageSyncReq{};
    imageSyncReq.stageMask = vk::PipelineStageFlagBits2::eTransfer;
    imageSyncReq.accessMask = vk::AccessFlagBits2::eTransferRead;
    imageSyncReq.layout = vk::ImageLayout::eTransferSrcOptimal;

    auto imageIt = m_imageSyncStates.find(textureView->rootTexture());
    if (imageIt != m_imageSyncStates.end()) {
        auto barrier = syncImage(imageIt->second, imageSyncReq); // will update the final sync state
        if (barrier.has_value()) {
            barrier->setImage(textureView->rootTexture()->vkImage());
            barrier->setSubresourceRange(textureView->subresourceRange());
            imageMemoryBarriers.push_back(*barrier);
        }
    } else {
        m_imageSyncRequests[textureView->rootTexture()] = imageSyncReq;
        m_imageSyncStates[textureView->rootTexture()] = imageStateAfterSync(imageSyncReq);
    }

    BufferSyncRequest bufferSyncReq{};
    bufferSyncReq.stageMask = vk::PipelineStageFlagBits2::eTransfer;
    bufferSyncReq.accessMask = vk::AccessFlagBits2::eTransferWrite;

    auto bufferIt = m_bufferFinalSyncStates.find(buffer);
    if (bufferIt != m_bufferFinalSyncStates.end()) {
        auto barrier = syncBuffer(bufferIt->second, bufferSyncReq); // will update the final sync state
        if (barrier.has_value()) {
            barrier->setBuffer(buffer->vkBuffer());
            barrier->setOffset(0);
            barrier->setSize(vk::WholeSize);
            bufferMemoryBarriers.push_back(*barrier);
        }
    } else {
        m_bufferSyncRequests[buffer] = bufferSyncReq;
        m_bufferFinalSyncStates[buffer] = bufferStateAfterSync(bufferSyncReq);
    }

    if (imageMemoryBarriers.empty() == false || bufferMemoryBarriers.empty() == false)
    {
        auto dependencyInfo = vk::DependencyInfo{}
            .setDependencyFlags(vk::DependencyFlags{});

        if (imageMemoryBarriers.empty() == false)
            dependencyInfo.setImageMemoryBarriers(imageMemoryBarriers);
        if (bufferMemoryBarriers.empty() == false)
            dependencyInfo.setBufferMemoryBarriers(bufferMemoryBarriers);

        m_vkCommandBuffer.pipelineBarrier2(dependencyInfo);
    }

    auto bufferImageCopy = vk::BufferImageCopy{}
        .setBufferOffset(bufferOffset)
        .setImageSubresource(vk::ImageSubresourceLayers{}
            .setAspectMask(textureView->subresourceRange().aspectMask)
            .setMipLevel(textureView->baseMipLevel())
            .setBaseArrayLayer(layerIndex)
            .setLayerCount(1))
        .setImageExtent(vk::Extent3D{}
            .setWidth(textureView->width())
            .setHeight(textureView->height())
            .setDepth(1));

    m_vkCommandBuffer.copyImageToBuffer(
        textureView->rootTexture()->vkImage(),
        vk::ImageLayout::eTransferSrcOptimal,
        buffer->vkBuffer(),
        bufferImageCopy);
}

void VulkanCommandBuffer::generateMipmaps(const std::shared_ptr<Texture>& aTexture)
{
    auto textureView = std::dynamic_pointer_cast<VulkanTextureView>(aTexture);
    assert(textureView);
    assert(textureView->mipLevelCount() > 1);
    assert(textureView->usages() & TextureUsage::copySource);
    assert(textureView->usages() & TextureUsage::copyDestination);
    assert(textureView->pixelFormat() != PixelFormat::RG32Uint);
    assert(textureView->pixelFormat() != PixelFormat::Depth32Float);

    const auto rootTexture = textureView->rootTexture();
    const auto& viewRange = textureView->subresourceRange();
    const auto rootRange = vk::ImageSubresourceRange{}
        .setAspectMask(viewRange.aspectMask)
        .setBaseMipLevel(0)
        .setLevelCount(rootTexture->mipLevelCount())
        .setBaseArrayLayer(0)
        .setLayerCount(rootTexture->type() == TextureType::textureCube ? 6u : rootTexture->arrayLayerCount());

    ImageSyncRequest sourceRequest{};
    sourceRequest.stageMask = vk::PipelineStageFlagBits2::eTransfer;
    sourceRequest.accessMask = vk::AccessFlagBits2::eTransferRead;
    sourceRequest.layout = vk::ImageLayout::eTransferSrcOptimal;
    sourceRequest.preserveContent = true;

    auto imageIt = m_imageSyncStates.find(rootTexture);
    if (imageIt != m_imageSyncStates.end()) {
        auto barrier = syncImage(imageIt->second, sourceRequest);
        if (barrier.has_value()) {
            barrier->setImage(rootTexture->vkImage());
            barrier->setSubresourceRange(rootRange);
            m_vkCommandBuffer.pipelineBarrier2(vk::DependencyInfo{}
                .setDependencyFlags(vk::DependencyFlags{})
                .setImageMemoryBarriers(*barrier));
        }
    } else {
        m_imageSyncRequests[rootTexture] = sourceRequest;
        m_imageSyncStates[rootTexture] = imageStateAfterSync(sourceRequest);
    }

    const auto transitionMip = [&](uint32_t mipLevel,
                                   vk::ImageLayout oldLayout,
                                   vk::ImageLayout newLayout,
                                   vk::AccessFlags2 srcAccessMask,
                                   vk::AccessFlags2 dstAccessMask)
    {
        const auto barrier = vk::ImageMemoryBarrier2{}
            .setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer)
            .setSrcAccessMask(srcAccessMask)
            .setDstStageMask(vk::PipelineStageFlagBits2::eTransfer)
            .setDstAccessMask(dstAccessMask)
            .setOldLayout(oldLayout)
            .setNewLayout(newLayout)
            .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setImage(rootTexture->vkImage())
            .setSubresourceRange(vk::ImageSubresourceRange{}
                .setAspectMask(viewRange.aspectMask)
                .setBaseMipLevel(mipLevel)
                .setLevelCount(1)
                .setBaseArrayLayer(viewRange.baseArrayLayer)
                .setLayerCount(viewRange.layerCount));

        m_vkCommandBuffer.pipelineBarrier2(vk::DependencyInfo{}
            .setDependencyFlags(vk::DependencyFlags{})
            .setImageMemoryBarriers(barrier));
    };

    for (uint32_t relativeMipLevel = 1; relativeMipLevel < viewRange.levelCount; relativeMipLevel++)
    {
        const uint32_t sourceMipLevel = viewRange.baseMipLevel + relativeMipLevel - 1;
        const uint32_t destinationMipLevel = viewRange.baseMipLevel + relativeMipLevel;

        transitionMip(
            destinationMipLevel,
            vk::ImageLayout::eTransferSrcOptimal,
            vk::ImageLayout::eTransferDstOptimal,
            vk::AccessFlagBits2::eTransferRead,
            vk::AccessFlagBits2::eTransferWrite);

        const auto sourceWidth = static_cast<int32_t>(std::max(rootTexture->width() >> sourceMipLevel, 1u));
        const auto sourceHeight = static_cast<int32_t>(std::max(rootTexture->height() >> sourceMipLevel, 1u));
        const auto destinationWidth = static_cast<int32_t>(std::max(rootTexture->width() >> destinationMipLevel, 1u));
        const auto destinationHeight = static_cast<int32_t>(std::max(rootTexture->height() >> destinationMipLevel, 1u));

        const auto blit = vk::ImageBlit{}
            .setSrcSubresource(vk::ImageSubresourceLayers{}
                .setAspectMask(viewRange.aspectMask)
                .setMipLevel(sourceMipLevel)
                .setBaseArrayLayer(viewRange.baseArrayLayer)
                .setLayerCount(viewRange.layerCount))
            .setSrcOffsets({
                vk::Offset3D{.x = 0, .y = 0, .z = 0},
                vk::Offset3D{.x = sourceWidth, .y = sourceHeight, .z = 1}})
            .setDstSubresource(vk::ImageSubresourceLayers{}
                .setAspectMask(viewRange.aspectMask)
                .setMipLevel(destinationMipLevel)
                .setBaseArrayLayer(viewRange.baseArrayLayer)
                .setLayerCount(viewRange.layerCount))
            .setDstOffsets({
                vk::Offset3D{.x = 0, .y = 0, .z = 0},
                vk::Offset3D{.x = destinationWidth, .y = destinationHeight, .z = 1}});

        m_vkCommandBuffer.blitImage(
            rootTexture->vkImage(),
            vk::ImageLayout::eTransferSrcOptimal,
            rootTexture->vkImage(),
            vk::ImageLayout::eTransferDstOptimal,
            blit,
            vk::Filter::eLinear);

        transitionMip(
            destinationMipLevel,
            vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::eTransferSrcOptimal,
            vk::AccessFlagBits2::eTransferWrite,
            vk::AccessFlagBits2::eTransferRead);
    }
}

void VulkanCommandBuffer::endBlitPass()
{
    assert(m_boundPipeline == nullptr);
}

void VulkanCommandBuffer::presentDrawable(const std::shared_ptr<Drawable>& aDrawable)
{
    auto drawable = std::dynamic_pointer_cast<VulkanDrawable>(aDrawable);
    assert(drawable);

    assert(m_boundPipeline == nullptr);

    m_presentedDrawables.insert(drawable);
}

void VulkanCommandBuffer::addSampledTexture(const std::shared_ptr<Texture>& aTexture)
{
    auto textureView = std::dynamic_pointer_cast<VulkanTextureView>(aTexture);
    assert(textureView);

    ImageSyncRequest syncReq{};
    syncReq.stageMask = vk::PipelineStageFlagBits2::eFragmentShader;
    syncReq.accessMask = vk::AccessFlagBits2::eShaderRead;
    syncReq.layout = vk::ImageLayout::eShaderReadOnlyOptimal;
    syncReq.preserveContent = true;

    auto it = m_imageSyncStates.find(textureView->rootTexture());
    if (it != m_imageSyncStates.end()) {
        auto barrier = syncImage(it->second, syncReq); // will update the final sync state
        if (barrier.has_value()) {
            barrier->setImage(textureView->rootTexture()->vkImage());
            barrier->setSubresourceRange(textureView->subresourceRange());
            m_vkCommandBuffer.pipelineBarrier2(vk::DependencyInfo{}
               .setDependencyFlags(vk::DependencyFlags{})
               .setImageMemoryBarriers(*barrier));
        }
    } else {
        m_imageSyncRequests[textureView->rootTexture()] = syncReq;
        m_imageSyncStates[textureView->rootTexture()] = imageStateAfterSync(syncReq);
    }
}

}
