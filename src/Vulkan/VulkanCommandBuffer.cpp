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
#include "Vulkan/VulkanDrawable.hpp"
#include "Vulkan/VulkanParameterBlock.hpp"
#include "Vulkan/VulkanSampler.hpp"
#include "Vulkan/VulkanTexture.hpp"
#include "Vulkan/VulkanEnums.hpp"
#if defined(GFX_IMGUI_ENABLED)
# include "Vulkan/imgui_impl_vulkan.h"
#endif
#include "Vulkan/VulkanGraphicsPipeline.hpp"
#include "Vulkan/VulkanCommandBufferPool.hpp"
#include "Vulkan/VulkanDevice.hpp"

namespace gfx
{

VulkanCommandBuffer::VulkanCommandBuffer(const VulkanDevice* device, const std::shared_ptr<vk::CommandPool>& commandPool, VulkanCommandBufferPool* sourcePool)
    : m_device(device), m_vkCommandPool(commandPool), m_sourcePool(sourcePool)
{
    auto commandBufferAllocateInfo = vk::CommandBufferAllocateInfo{}
        .setCommandPool(*m_vkCommandPool)
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandBufferCount(1);
    m_vkCommandBuffer = m_device->vkDevice().allocateCommandBuffers(commandBufferAllocateInfo).front();
}

CommandBufferPool* VulkanCommandBuffer::pool()
{
    return m_sourcePool;
}

void VulkanCommandBuffer::beginRenderPass(const Framebuffer& framebuffer)
{
    std::vector<vk::RenderingAttachmentInfo> colorAttachmentInfos(framebuffer.colorAttachments.size());
    std::optional<vk::RenderingAttachmentInfo> depthAttachmentInfo;
    std::vector<vk::ImageMemoryBarrier2> imageMemoryBarriers;

    for (size_t i = 0; auto& colorAttachment : framebuffer.colorAttachments)
    {
        std::shared_ptr<VulkanTexture> texture = dynamic_pointer_cast<VulkanTexture>(colorAttachment.texture);
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
        std::shared_ptr<VulkanTexture> texture = dynamic_pointer_cast<VulkanTexture>(depthAttachment->texture);
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

    m_vkCommandBuffer.setViewport(0, vk::Viewport{}
        .setX(0)
        .setY(0)
        .setWidth(static_cast<float>(framebuffer.colorAttachments[0].texture->width()))
        .setHeight(static_cast<float>(framebuffer.colorAttachments[0].texture->height()))
        .setMinDepth(0)
        .setMaxDepth(1));

    m_vkCommandBuffer.setScissor(0, vk::Rect2D{}
        .setOffset({.x=0, .y=0})
        .setExtent({
            .width = framebuffer.colorAttachments[0].texture->width(),
            .height = framebuffer.colorAttachments[0].texture->height()
        }));

    auto renderingInfo = vk::RenderingInfo{}
        .setRenderArea(vk::Rect2D{}
            .setOffset({.x=0, .y=0})
            .setExtent({
                .width = framebuffer.colorAttachments[0].texture->width(),
                .height = framebuffer.colorAttachments[0].texture->height()
            }))
        .setLayerCount(1)
        .setViewMask(0)
        .setColorAttachments(colorAttachmentInfos)
        .setPDepthAttachment(depthAttachmentInfo ? &depthAttachmentInfo.value() : nullptr);

    m_vkCommandBuffer.beginRendering(renderingInfo);
}

void VulkanCommandBuffer::usePipeline(const std::shared_ptr<const GraphicsPipeline>& _graphicsPipeline)
{
    auto graphicsPipeline = std::dynamic_pointer_cast<const VulkanGraphicsPipeline>(_graphicsPipeline);

    m_vkCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline->vkPipeline());

    m_usedPipelines.insert(graphicsPipeline);
    m_boundPipeline = graphicsPipeline.get();
}

void VulkanCommandBuffer::useVertexBuffer(const std::shared_ptr<Buffer>& aBuffer)
{
    auto buffer = std::dynamic_pointer_cast<VulkanBuffer>(aBuffer);

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
        auto [it1, res1] = m_bufferSyncRequests.insert(std::make_pair(buffer, syncReq));
        assert(res1);
        (void)res1;
        auto [it2, res2] = m_bufferFinalSyncStates.insert(std::make_pair(buffer, bufferStateAfterSync(syncReq)));
        assert(res2);
        (void)res2;
    }

    m_vkCommandBuffer.bindVertexBuffers(0, buffer->vkBuffer(), {0});
}

void VulkanCommandBuffer::setParameterBlock(const std::shared_ptr<const ParameterBlock>& aPblock, uint32_t index)
{
    const auto& pBlock = std::dynamic_pointer_cast<const VulkanParameterBlock>(aPblock);
    std::vector<vk::BufferMemoryBarrier2> bufferMemoryBarriers;
    std::vector<vk::ImageMemoryBarrier2> imageMemoryBarriers;

    for (auto& [buffer, binding] : pBlock->usedBuffers())
    {
        BufferSyncRequest syncReq{};

        if ((binding.usages & BindingUsage::vertexRead) || (binding.usages & BindingUsage::vertexWrite))
            syncReq.stageMask |= vk::PipelineStageFlagBits2::eVertexShader;
        if ((binding.usages & BindingUsage::fragmentRead) || (binding.usages & BindingUsage::fragmentWrite))
            syncReq.stageMask |= vk::PipelineStageFlagBits2::eFragmentShader;

        if (static_cast<bool>(binding.usages & (BindingUsage::vertexRead | BindingUsage::fragmentRead))) {
            assert(binding.type == BindingType::uniformBuffer); // currently only uniform buffer are implemented
            syncReq.accessMask |= vk::AccessFlagBits2::eUniformRead;
        }
        if (static_cast<bool>(binding.usages & (BindingUsage::vertexWrite | BindingUsage::fragmentWrite))) {
            throw std::runtime_error("not implemented");
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
            auto [it1, res1] = m_bufferSyncRequests.insert(std::make_pair(buffer, syncReq));
            assert(res1);
            (void)res1;
            auto [it2, res2] = m_bufferFinalSyncStates.insert(std::make_pair(buffer, bufferStateAfterSync(syncReq)));
            assert(res2);
            (void)res2;
        }
    }

    for (auto& [texture, binding] : pBlock->usedTextures())
    {
        ImageSyncRequest syncReq{};

        if ((binding.usages & BindingUsage::vertexRead) || (binding.usages & BindingUsage::vertexWrite))
            syncReq.stageMask |= vk::PipelineStageFlagBits2::eVertexShader;
        if ((binding.usages & BindingUsage::fragmentRead) || (binding.usages & BindingUsage::fragmentWrite))
            syncReq.stageMask |= vk::PipelineStageFlagBits2::eFragmentShader;

        if (static_cast<bool>(binding.usages & (BindingUsage::vertexRead | BindingUsage::fragmentRead))) {
            assert(binding.type == BindingType::sampledTexture);
            syncReq.accessMask |= vk::AccessFlagBits2::eShaderRead;
            syncReq.layout = vk::ImageLayout::eShaderReadOnlyOptimal;
            syncReq.preserveContent = true;
        }
        if (static_cast<bool>(binding.usages & (BindingUsage::vertexWrite | BindingUsage::fragmentWrite))) {
            syncReq.accessMask |= vk::AccessFlagBits2::eShaderWrite;
            syncReq.layout = vk::ImageLayout::eGeneral; // allow read and write;
            syncReq.preserveContent = true;
            throw std::runtime_error("not implemented"); // never tested, because never had use case
        }

        auto it = m_imageFinalSyncStates.find(texture);
        if (it != m_imageFinalSyncStates.end()) {
            auto barrier = syncImage(it->second, syncReq); // will update the final sync state
            if (barrier.has_value()) {
                barrier->setImage(texture->vkImage());
                barrier->setSubresourceRange(texture->subresourceRange());
                imageMemoryBarriers.push_back(*barrier);
            }
        } else {
            auto [it1, res1] = m_imageSyncRequests.insert(std::make_pair(texture, syncReq));
            assert(res1);
            (void)res1;
            auto [it2, res2] = m_imageFinalSyncStates.insert(std::make_pair(texture, imageStateAfterSync(syncReq)));
            assert(res2);
            (void)res2;
        }
    }

#ifdef __cpp_lib_containers_ranges
    m_usedSamplers.insert_range(pBlock->usedSamplers() | std::views::transform([](auto& pair) { return pair.first; }));
#else
    auto rg = pBlock->usedSamplers() | std::views::transform([](auto& pair) { return pair.first; });
    m_usedSamplers.insert(rg.cbegin(), rg.cend());
#endif


    if (bufferMemoryBarriers.empty() == false)
    {
        auto dependencyInfo = vk::DependencyInfo{}
            .setDependencyFlags(vk::DependencyFlags{})
            .setBufferMemoryBarriers(bufferMemoryBarriers);

        m_vkCommandBuffer.pipelineBarrier2(dependencyInfo);
    }

    assert(m_boundPipeline != nullptr);
    m_vkCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_boundPipeline->pipelineLayout(), index, pBlock->descriptorSet(), {});

    m_usedPBlock.insert(pBlock);
}

void VulkanCommandBuffer::setPushConstants(const void* data, size_t size)
{
    assert(m_boundPipeline != nullptr);
    m_vkCommandBuffer.pushConstants(m_boundPipeline->pipelineLayout(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0, size, data);
}

void VulkanCommandBuffer::drawVertices(uint32_t start, uint32_t count)
{
    m_vkCommandBuffer.draw(count, 1, start, 0);
}

void VulkanCommandBuffer::drawIndexedVertices(const std::shared_ptr<Buffer>& aBuffer)
{
    auto buffer = std::dynamic_pointer_cast<VulkanBuffer>(aBuffer);

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
        auto [it1, res1] = m_bufferSyncRequests.insert(std::make_pair(buffer, syncReq));
        assert(res1);
        (void)res1;
        auto [it2, res2] = m_bufferFinalSyncStates.insert(std::make_pair(buffer, bufferStateAfterSync(syncReq)));
        assert(res2);
        (void)res2;
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

void VulkanCommandBuffer::beginBlitPass()
{
    // nothing
}

void VulkanCommandBuffer::copyBufferToBuffer(const std::shared_ptr<Buffer>& aSrc, const std::shared_ptr<Buffer>& aDst, size_t size)
{
    auto src = std::dynamic_pointer_cast<VulkanBuffer>(aSrc);
    assert(src);
    auto dst = std::dynamic_pointer_cast<VulkanBuffer>(aDst);
    assert(dst);

    assert(src->usages() & BufferUsage::copySource);
    assert(dst->usages() & BufferUsage::copyDestination);

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
        auto [it1, res1] = m_bufferSyncRequests.insert(std::make_pair(src, srcBufferSyncReq));
        assert(res1);
        (void)res1;
        auto [it2, res2] = m_bufferFinalSyncStates.insert(std::make_pair(src, bufferStateAfterSync(srcBufferSyncReq)));
        assert(res2);
        (void)res2;
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
        auto [it1, res1] = m_bufferSyncRequests.insert(std::make_pair(dst, dstBufferSyncReq));
        assert(res1);
        (void)res1;
        auto [it2, res2] = m_bufferFinalSyncStates.insert(std::make_pair(dst, bufferStateAfterSync(dstBufferSyncReq)));
        assert(res2);
        (void)res2;
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
    auto texture = std::dynamic_pointer_cast<VulkanTexture>(aTexture);
    assert(texture);

    assert(buffer->usages() & BufferUsage::copySource);
    assert(texture->usages() & TextureUsage::copyDestination);
    assert(bufferOffset + pixelFormatSize(texture->pixelFormat()) * texture->width() * texture->height() <= buffer->size());

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
        auto [it1, res1] = m_bufferSyncRequests.insert(std::make_pair(buffer, bufferSyncReq));
        assert(res1);
        (void)res1;
        auto [it2, res2] = m_bufferFinalSyncStates.insert(std::make_pair(buffer, bufferStateAfterSync(bufferSyncReq)));
        assert(res2);
        (void)res2;
    }

    ImageSyncRequest imageSyncReq{};
    imageSyncReq.stageMask = vk::PipelineStageFlagBits2::eTransfer;
    imageSyncReq.accessMask = vk::AccessFlagBits2::eTransferWrite;
    imageSyncReq.layout = vk::ImageLayout::eTransferDstOptimal;
    imageSyncReq.preserveContent = false;

    auto it2 = m_imageFinalSyncStates.find(texture);
    if (it2 != m_imageFinalSyncStates.end()) {
        auto barrier = syncImage(it2->second, imageSyncReq); // will update the final sync state
        if (barrier.has_value()) {
            barrier->setImage(texture->vkImage());
            barrier->setSubresourceRange(texture->subresourceRange());
            imageMemoryBarriers.push_back(*barrier);
        }
    } else {
        auto [it1, res1] = m_imageSyncRequests.insert(std::make_pair(texture, imageSyncReq));
        assert(res1);
        (void)res1;
        auto [it2, res2] = m_imageFinalSyncStates.insert(std::make_pair(texture, imageStateAfterSync(imageSyncReq)));
        assert(res2);
        (void)res2;
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
            .setAspectMask(texture->subresourceRange().aspectMask)
            .setMipLevel(0)
            .setBaseArrayLayer(layerIndex)
            .setLayerCount(1))
        .setImageExtent(vk::Extent3D{}
            .setWidth(texture->width())
            .setHeight(texture->height())
            .setDepth(1));

    m_vkCommandBuffer.copyBufferToImage(
        buffer->vkBuffer(),
        texture->vkImage(),
        vk::ImageLayout::eTransferDstOptimal,
        bufferImageCopy);
}

void VulkanCommandBuffer::endBlitPass()
{
    // nothing
}

void VulkanCommandBuffer::presentDrawable(const std::shared_ptr<Drawable>& aDrawable)
{
    auto drawable = std::dynamic_pointer_cast<VulkanDrawable>(aDrawable);
    m_presentedDrawables.insert(drawable);
}

void VulkanCommandBuffer::reuse()
{
    m_presentedDrawables.clear();
    m_usedPBlock.clear();
    m_bufferFinalSyncStates.clear();
    m_bufferSyncRequests.clear();
    m_imageFinalSyncStates.clear();
    m_imageSyncRequests.clear();
    m_boundPipeline = nullptr;
    m_usedPipelines.clear();
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
    if (m_sourcePool)
        m_sourcePool->release(this);
    m_device->vkDevice().freeCommandBuffers(*m_vkCommandPool, m_vkCommandBuffer);
}

}
