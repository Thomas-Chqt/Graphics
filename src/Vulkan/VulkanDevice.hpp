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
#include "Graphics/Swapchain.hpp"
#include "Graphics/CommandBuffer.hpp"
#include "Graphics/Drawable.hpp"

#include "Vulkan/QueueFamily.hpp"
#include "Vulkan/VulkanCommandBuffer.hpp"
#include "Vulkan/VulkanDrawable.hpp"
#include "Vulkan/VulkanTexture.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    #include <memory>
    #include <cstddef>
    #include <queue>
    #include <cstdint>
    namespace ext = std;
#endif

namespace gfx
{

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

    VulkanDevice(const VulkanPhysicalDevice*, const Descriptor&);

    ext::unique_ptr<Swapchain> newSwapchain(const Swapchain::Descriptor&) const override;
    ext::unique_ptr<ShaderLib> newShaderLib(const ext::filesystem::path&) const override;
    ext::unique_ptr<GraphicsPipeline> newGraphicsPipeline(const GraphicsPipeline::Descriptor&) const override;
    ext::unique_ptr<Buffer> newBuffer(const Buffer::Descriptor&) const override;

    void beginFrame(void) override;

    CommandBuffer& commandBuffer(void) override;
 
    void submitCommandBuffer(CommandBuffer&) override;
    void presentDrawable(const ext::shared_ptr<Drawable>&) override;

    void endFrame(void) override;

    void waitIdle(void) override;

    inline const vk::Device& vkDevice(void) const { return m_vkDevice; }
    inline const VulkanPhysicalDevice& physicalDevice(void) const { return *m_physicalDevice; }
    inline uint32_t maxFrameInFlight(void) const { return m_frameData.size(); };

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
    };

    const VulkanPhysicalDevice* m_physicalDevice;
    vk::Device m_vkDevice;
    QueueFamily m_queueFamily;
    vk::Queue m_queue;
    
    ext::vector<FrameData> m_frameData;
    ext::vector<FrameData>::iterator m_currFD;

public:
    VulkanDevice& operator=(const VulkanDevice&) = delete;
    VulkanDevice& operator=(VulkanDevice&&) = delete;
};

} // namespace gfx

#endif // VULKANDEVICE_HPP
