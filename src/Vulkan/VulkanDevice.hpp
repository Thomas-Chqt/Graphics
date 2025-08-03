/*
 * ---------------------------------------------------
 * VulkanDevice.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/23 07:29:33
 * ---------------------------------------------------
 */

#ifndef VULKANDEVICE_HPP
#define VULKANDEVICE_HPP

#include "Graphics/Device.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Swapchain.hpp"
#include "Graphics/CommandBuffer.hpp"
#include "Graphics/Drawable.hpp"

#include "Vulkan/QueueFamily.hpp"
#include "Vulkan/VulkanCommandBuffer.hpp"
#include "Vulkan/VulkanDrawable.hpp"
#include "Vulkan/VulkanTexture.hpp"

#include <vulkan/vulkan.hpp>
#include "Vulkan/vk_mem_alloc.hpp" // IWYU pragma: keep

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    #include <memory>
    #include <cstddef>
    #include <queue>
    #include <cstdint>
    #include <iterator>
    namespace ext = std;
#endif

namespace gfx
{

class VulkanInstance;
class VulkanPhysicalDevice;

class VulkanDevice : public Device
{
public:
    struct Descriptor
    {
        const Device::Descriptor* deviceDescriptor;
        ext::vector<const char*> deviceExtensions;
    };

public:
    VulkanDevice() = delete;
    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice(VulkanDevice&&) = delete;

    VulkanDevice(const VulkanInstance*, const VulkanPhysicalDevice*, const Descriptor&);

    ext::unique_ptr<Swapchain> newSwapchain(const Swapchain::Descriptor&) const override;
    ext::unique_ptr<ShaderLib> newShaderLib(const ext::filesystem::path&) const override;
    ext::unique_ptr<GraphicsPipeline> newGraphicsPipeline(const GraphicsPipeline::Descriptor&) const override;
    ext::unique_ptr<Buffer> newBuffer(const Buffer::Descriptor&) const override;

#if defined(GFX_IMGUI_ENABLED)
    void imguiInit(uint32_t imageCount,
                   ext::vector<PixelFormat> colorAttachmentPxFormats,
                   ext::optional<PixelFormat> depthAttachmentPxFormat) const override;
#endif

#if defined(GFX_IMGUI_ENABLED)
    void imguiNewFrame() const override;
#endif

    void beginFrame(void) override;

    CommandBuffer& commandBuffer(void) override;
 
    void submitCommandBuffer(CommandBuffer&) override;
    void presentDrawable(const ext::shared_ptr<Drawable>&) override;

    void endFrame(void) override;

    void waitIdle(void) override;

    inline Backend backend() const override { return Backend::vulkan; }
    inline uint32_t maxFrameInFlight() const override { return static_cast<uint32_t>(m_frameDatas.size()); };
    inline uint32_t currentFrameIdx() const override { return static_cast<uint32_t>(ext::distance(m_frameDatas.begin(), ext::vector<FrameData>::const_iterator(m_currFrameData))); };

    inline const vk::Device& vkDevice(void) const { return m_vkDevice; }
    inline const VulkanPhysicalDevice& physicalDevice(void) const { return *m_physicalDevice; }
    inline const VmaAllocator& allocator() const { return m_allocator; }

#if defined(GFX_IMGUI_ENABLED)
    void imguiShutdown() const override;
#endif

    ~VulkanDevice();

private:
    struct FrameData
    {
        vk::CommandPool commandPool;
        vk::Fence frameCompletedFence;
        ext::queue<VulkanCommandBuffer> commandBuffers;
        ext::queue<VulkanCommandBuffer> usedCommandBuffers;
        ext::vector<VulkanCommandBuffer*> submittedCmdBuffers;
        ext::vector<ext::shared_ptr<VulkanDrawable>> presentedDrawables;

        FrameData() = default;
        FrameData(const FrameData&) = delete;
        FrameData(FrameData&&) = default;
    };
    
    const VulkanInstance* const m_instance;
    const VulkanPhysicalDevice* const m_physicalDevice;
    vk::Device m_vkDevice;
    QueueFamily m_queueFamily;
    vk::Queue m_queue;
    VmaAllocator m_allocator;
    
    ext::vector<FrameData> m_frameDatas;
    ext::vector<FrameData>::iterator m_currFrameData;

public:
    VulkanDevice& operator=(const VulkanDevice&) = delete;
    VulkanDevice& operator=(VulkanDevice&&) = delete;
};

} // namespace gfx

#endif // VULKANDEVICE_HPP
