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

#include "Vulkan/QueueFamily.hpp"
#include "Vulkan/VulkanBuffer.hpp"
#include "Vulkan/VulkanGraphicsPipeline.hpp"
#include "Vulkan/VulkanTexture.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    #include <cstdint>
    #include <map>
    #include <set>
    #include <vector>
    namespace ext = std;
#endif

namespace gfx
{

class VulkanCommandBuffer : public CommandBuffer
{
public:
    VulkanCommandBuffer() = default;
    VulkanCommandBuffer(const VulkanCommandBuffer&) = delete;
    VulkanCommandBuffer(VulkanCommandBuffer&&) = default;

    VulkanCommandBuffer(vk::CommandBuffer&&, const QueueFamily&);

    void beginRenderPass(const Framebuffer&) override;

    void usePipeline(const ext::shared_ptr<const GraphicsPipeline>&) override;
    void useVertexBuffer(const ext::shared_ptr<Buffer>&) override;

    void setParameterBlock(const ParameterBlock&, uint32_t index) override;

    void drawVertices(uint32_t start, uint32_t count) override;
    void drawIndexedVertices(const ext::shared_ptr<Buffer>& idxBuffer) override;

#if defined(GFX_IMGUI_ENABLED)
    void imGuiRenderDrawData(ImDrawData*) const override;
#endif

    void endRenderPass() override;

    const vk::CommandBuffer& vkCommandBuffer() const { return m_vkCommandBuffer; }

    inline void begin() { m_vkCommandBuffer.begin(vk::CommandBufferBeginInfo{.flags=vk::CommandBufferUsageFlagBits::eOneTimeSubmit}); }
    inline void end() { m_vkCommandBuffer.end(); }

    inline const ext::map<ext::shared_ptr<VulkanTexture>, ImageSyncRequest>& imageSyncRequests() const { return m_imageSyncRequests; }
    inline const ext::map<ext::shared_ptr<VulkanTexture>, ImageSyncState>& imageFinalSyncStates() const { return m_imageFinalSyncStates; }
    inline const ext::map<ext::shared_ptr<VulkanBuffer>, BufferSyncRequest>& bufferSyncRequests() const { return m_bufferSyncRequests; }
    inline const ext::map<ext::shared_ptr<VulkanBuffer>, BufferSyncState>& bufferFinalSyncStates() const { return m_bufferFinalSyncStates; }

    inline ext::set<const vk::Semaphore*>& waitSemaphores() { return m_waitSemaphores; }
    inline const ext::set<const vk::Semaphore*>& waitSemaphores() const { return m_waitSemaphores; }

    inline const vk::Semaphore*& signalSemaphore() { return m_signalSemaphore; }
    inline const vk::Semaphore* signalSemaphore() const { return m_signalSemaphore; }

    void clear();

    ~VulkanCommandBuffer() override = default;

private:
    vk::CommandBuffer m_vkCommandBuffer;
    QueueFamily m_queueFamily;

    vk::Viewport m_viewport;
    vk::Rect2D m_scissor;

    ext::vector<ext::shared_ptr<const VulkanGraphicsPipeline>> m_usedPipelines;

    ext::map<ext::shared_ptr<VulkanTexture>, ImageSyncRequest> m_imageSyncRequests;
    ext::map<ext::shared_ptr<VulkanTexture>, ImageSyncState> m_imageFinalSyncStates;

    ext::map<ext::shared_ptr<VulkanBuffer>, BufferSyncRequest> m_bufferSyncRequests;
    ext::map<ext::shared_ptr<VulkanBuffer>, BufferSyncState> m_bufferFinalSyncStates;

    ext::set<const vk::Semaphore*> m_waitSemaphores;
    const vk::Semaphore* m_signalSemaphore = nullptr;

public:
    VulkanCommandBuffer& operator = (const VulkanCommandBuffer&) = delete;
    VulkanCommandBuffer& operator = (VulkanCommandBuffer&&) = default;
};

}

#endif // VULKANCOMMANDBUFFER_HPP
