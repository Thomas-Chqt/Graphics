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

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    #include <memory>
    namespace ext = std;
#endif

constexpr int MAX_FRAMES_IN_FLIGHT = 1;

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

    ext::unique_ptr<Swapchain> newSwapchain(const Swapchain::Descriptor&) const override;

    void beginFrame(void) override;

    ext::unique_ptr<CommandBuffer> commandBuffer(void) override;
 
    void submitCommandBuffer(ext::unique_ptr<CommandBuffer>&&) override;
    void presentDrawable(const ext::shared_ptr<Drawable>&) override;

    void endFrame(void) override;

    void waitIdle(void) override;

    inline const vk::Device& vkDevice(void) const { return m_vkDevice; }
    inline const VulkanPhysicalDevice& physicalDevice(void) const { return *m_physicalDevice; }

    ~VulkanDevice();

private:
    const VulkanPhysicalDevice* m_physicalDevice;
    vk::Device m_vkDevice;
    QueueFamily m_queueFamily;
    vk::Queue m_queue;

    vk::CommandPool m_commandPool;
    vk::Fence m_frameCompletedFence;
    ext::vector<vk::CommandBuffer> m_vkCommandBuffers;
    int m_nextVkCommandBuffer = 0;
    ext::vector<ext::unique_ptr<VulkanCommandBuffer>> m_submittedCommandBuffers;
    ext::vector<ext::shared_ptr<VulkanDrawable>> m_presentedDrawables;

public:
    VulkanDevice& operator=(const VulkanDevice&) = delete;
    VulkanDevice& operator=(VulkanDevice&&) = delete;
};

} // namespace gfx

#endif // VULKANDEVICE_HPP
