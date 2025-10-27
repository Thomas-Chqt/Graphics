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
#include "Graphics/GraphicsPipeline.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/ParameterBlock.hpp"

#include "Vulkan/VulkanBuffer.hpp"
#include "Vulkan/VulkanGraphicsPipeline.hpp"
#include "Vulkan/VulkanSampler.hpp"
#include "Vulkan/VulkanTexture.hpp"
#include "Vulkan/VulkanDrawable.hpp"
#include "Vulkan/VulkanParameterBlock.hpp"

namespace gfx
{

class VulkanDevice;
class VulkanCommandBufferPool;

class VulkanCommandBuffer : public CommandBuffer
{
public:
    VulkanCommandBuffer() = delete;
    VulkanCommandBuffer(const VulkanCommandBuffer&) = delete;
    VulkanCommandBuffer(VulkanCommandBuffer&&) = delete;

    VulkanCommandBuffer(const VulkanDevice*, const std::shared_ptr<vk::CommandPool>&, VulkanCommandBufferPool*);

    CommandBufferPool* pool() override;
    VulkanCommandBufferPool* poolVulkan();

    void beginRenderPass(const Framebuffer&) override;

    void usePipeline(const std::shared_ptr<const GraphicsPipeline>&) override;
    void useVertexBuffer(const std::shared_ptr<Buffer>&) override;

    void setParameterBlock(const std::shared_ptr<const ParameterBlock>&, uint32_t index) override;

    void drawVertices(uint32_t start, uint32_t count) override;
    void drawIndexedVertices(const std::shared_ptr<Buffer>& idxBuffer) override;

#if defined(GFX_IMGUI_ENABLED)
    void imGuiRenderDrawData(ImDrawData*) const override;
#endif

    void endRenderPass() override;

    void beginBlitPass() override;

    void copyBufferToBuffer(const std::shared_ptr<Buffer>& src, const std::shared_ptr<Buffer>& dst, size_t size) override;
    void copyBufferToTexture(const std::shared_ptr<Buffer>& buffer, size_t bufferOffset, const std::shared_ptr<Texture>& texture, uint32_t layerIndex = 0) override;

    void endBlitPass() override;

    void presentDrawable(const std::shared_ptr<Drawable>&) override;

    const vk::CommandBuffer& vkCommandBuffer() const { return m_vkCommandBuffer; }

    inline void begin() { m_vkCommandBuffer.begin(vk::CommandBufferBeginInfo{.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit}); }
    inline void end() { m_vkCommandBuffer.end(); }

    inline const std::map<std::shared_ptr<VulkanTexture>, ImageSyncRequest>& imageSyncRequests() const { return m_imageSyncRequests; }
    inline const std::map<std::shared_ptr<VulkanTexture>, ImageSyncState>& imageFinalSyncStates() const { return m_imageFinalSyncStates; }
    inline const std::map<std::shared_ptr<VulkanBuffer>, BufferSyncRequest>& bufferSyncRequests() const { return m_bufferSyncRequests; }
    inline const std::map<std::shared_ptr<VulkanBuffer>, BufferSyncState>& bufferFinalSyncStates() const { return m_bufferFinalSyncStates; }
    inline const std::set<std::shared_ptr<VulkanDrawable>> presentedDrawables() const { return m_presentedDrawables; }

    inline void clearSourcePool() { m_sourcePool = nullptr; }
    void reuse(); // used to clear ressource but keep m_vkCommandBuffer so the buffer can be reused

    inline void setSignaledTimeValue(uint64_t v) { m_signaledTimeValue = v; }
    inline const uint64_t& signaledTimeValue() const { return m_signaledTimeValue; }

    ~VulkanCommandBuffer() override;

private:
    const VulkanDevice* m_device;
    std::shared_ptr<vk::CommandPool> m_vkCommandPool;
    VulkanCommandBufferPool* m_sourcePool;

    vk::CommandBuffer m_vkCommandBuffer;

    std::set<std::shared_ptr<const VulkanGraphicsPipeline>> m_usedPipelines;
    const VulkanGraphicsPipeline* m_boundPipeline = nullptr;

    std::map<std::shared_ptr<VulkanTexture>, ImageSyncRequest> m_imageSyncRequests;
    std::map<std::shared_ptr<VulkanTexture>, ImageSyncState> m_imageFinalSyncStates;

    std::map<std::shared_ptr<VulkanBuffer>, BufferSyncRequest> m_bufferSyncRequests;
    std::map<std::shared_ptr<VulkanBuffer>, BufferSyncState> m_bufferFinalSyncStates;

    std::set<std::shared_ptr<VulkanSampler>> m_usedSamplers;

    std::set<std::shared_ptr<const VulkanParameterBlock>> m_usedPBlock;

    std::set<std::shared_ptr<VulkanDrawable>> m_presentedDrawables;

    uint64_t m_signaledTimeValue = 0;

public:
    VulkanCommandBuffer& operator=(const VulkanCommandBuffer&) = delete;
    VulkanCommandBuffer& operator=(VulkanCommandBuffer&&) = delete;
};

} // namespace gfx

#endif // VULKANCOMMANDBUFFER_HPP
