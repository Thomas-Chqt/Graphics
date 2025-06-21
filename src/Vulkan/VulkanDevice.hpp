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
#include "Graphics/RenderPass.hpp"
#include "Graphics/Swapchain.hpp"
#include "Graphics/CommandBuffer.hpp"

#include "Vulkan/QueueFamily.hpp"
#include "Vulkan/VulkanCommandBuffer.hpp"
#include "Vulkan/VulkanSwapchain.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    #include <memory>
    namespace ext = std;
#endif

constexpr int MAX_FRAMES_IN_FLIGHT = 3;

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

    VulkanDevice(const VulkanPhysicalDevice&, const Descriptor&);

    ext::unique_ptr<RenderPass> newRenderPass(const RenderPass::Descriptor&) const override;
    ext::unique_ptr<Swapchain> newSwapchain(const Swapchain::Descriptor&) const override;

    void beginFrame(void) override;

    CommandBuffer& commandBuffer(void) override;
 
    void submitCommandBuffer(const CommandBuffer&) override;
    void presentSwapchain(const Swapchain&) override;

    void endFrame(void) override;

    inline const vk::Device& vkDevice(void) const { return m_vkDevice; }
    inline const VulkanPhysicalDevice& physicalDevice(void) const { return *m_physicalDevice; }
    inline const vk::Semaphore& imageAvailableSemaphore(void) const { return m_imageAvailableSemaphore; }

    ~VulkanDevice();

private:
    const VulkanPhysicalDevice* m_physicalDevice;
    vk::Device m_vkDevice;
    QueueFamily m_queueFamily;
    vk::Queue m_queue;

    vk::CommandPool m_commandPool;
    VulkanCommandBuffer m_commandBuffer;
    const VulkanSwapchain* m_presentedSwapchain;
    vk::Semaphore m_imageAvailableSemaphore;
    vk::Semaphore m_renderFinisedSemaphore;
    vk::Fence m_renderFinisedFence;

public:
    VulkanDevice& operator=(const VulkanDevice&) = delete;
    VulkanDevice& operator=(VulkanDevice&&) = delete;
};

} // namespace gfx

#endif // VULKANDEVICE_HPP
