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

class VulkanCommandBuffer : public CommandBuffer
{
public:
    VulkanCommandBuffer() = delete;
    VulkanCommandBuffer(const VulkanCommandBuffer&) = delete;
    VulkanCommandBuffer(VulkanCommandBuffer&&) = delete;

    VulkanCommandBuffer(const VulkanDevice*, const std::shared_ptr<vk::CommandPool>&);
    VulkanCommandBuffer(const VulkanDevice*, const vk::CommandPool&);

    void beginRenderPass(const Framebuffer&) override;

    void usePipeline(const std::shared_ptr<const GraphicsPipeline>&) override;
    void useVertexBuffer(const std::shared_ptr<Buffer>&) override;

    void setParameterBlock(const std::shared_ptr<const ParameterBlock>&, uint32_t index) override;
    void setPushConstants(const void* data, size_t size) override;

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

    inline const std::map<std::shared_ptr<VulkanTexture>, ImageSyncRequest>& imageSyncRequests() const { return m_nonReusedRessources.imageSyncRequests; }
    inline const std::map<std::shared_ptr<VulkanTexture>, ImageSyncState>& imageFinalSyncStates() const { return m_nonReusedRessources.imageFinalSyncStates; }
    inline const std::map<std::shared_ptr<VulkanBuffer>, BufferSyncRequest>& bufferSyncRequests() const { return m_nonReusedRessources.bufferSyncRequests; }
    inline const std::map<std::shared_ptr<VulkanBuffer>, BufferSyncState>& bufferFinalSyncStates() const { return m_nonReusedRessources.bufferFinalSyncStates; }

    inline const std::set<std::shared_ptr<VulkanDrawable>> presentedDrawables() const { return m_nonReusedRessources.presentedDrawables; }

    inline void reuse() { m_nonReusedRessources = NonReusedRessources(); }

    inline void setSignaledTimeValue(uint64_t v) { m_nonReusedRessources.signaledTimeValue = v; }
    inline const uint64_t& signaledTimeValue() const { return m_nonReusedRessources.signaledTimeValue; }

    ~VulkanCommandBuffer() override = default;

private:
    const VulkanDevice* m_device;
    std::shared_ptr<vk::CommandPool> m_vkCommandPool;

    vk::CommandBuffer m_vkCommandBuffer;

    struct NonReusedRessources
    {
        std::set<std::shared_ptr<const VulkanGraphicsPipeline>> usedPipelines;
        const VulkanGraphicsPipeline* boundPipeline = nullptr;

        std::set<std::shared_ptr<const VulkanParameterBlock>> usedPBlock;

        std::map<std::shared_ptr<VulkanTexture>, ImageSyncRequest> imageSyncRequests;
        std::map<std::shared_ptr<VulkanTexture>, ImageSyncState> imageFinalSyncStates;

        std::map<std::shared_ptr<VulkanBuffer>, BufferSyncRequest> bufferSyncRequests;
        std::map<std::shared_ptr<VulkanBuffer>, BufferSyncState> bufferFinalSyncStates;

        std::set<std::shared_ptr<VulkanDrawable>> presentedDrawables;

        uint64_t signaledTimeValue = 0;
    }
    m_nonReusedRessources;

public:
    VulkanCommandBuffer& operator=(const VulkanCommandBuffer&) = delete;
    VulkanCommandBuffer& operator=(VulkanCommandBuffer&&) = delete;
};

} // namespace gfx

#endif // VULKANCOMMANDBUFFER_HPP
