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

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    #include <map>
    #include <memory>
    #include <thread>
    #include <mutex>
    #include <array>
    namespace ext = std;
#endif

constexpr int MAX_FRAMES_IN_FLIGHT = 3;

using CommandPools = ext::array<vk::CommandPool, MAX_FRAMES_IN_FLIGHT>;

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
    ext::unique_ptr<CommandBuffer> newCommandBuffer() override;

    inline const vk::Device& vkDevice(void) const { return m_vkDevice; }
    inline const VulkanPhysicalDevice& physicalDevice(void) const { return *m_physicalDevice; }

    ~VulkanDevice();

private:
    CommandPools& makeThreadCommandPools(ext::thread::id);

    const VulkanPhysicalDevice* m_physicalDevice;
    vk::Device m_vkDevice;
    vk::Queue m_queue;
    QueueFamily m_queueFamily;

    int m_frameIndex = 0;

    ext::map<ext::thread::id, CommandPools> m_commandPools;
    ext::mutex m_commandPoolsMutex;

public:
    VulkanDevice& operator=(const VulkanDevice&) = delete;
    VulkanDevice& operator=(VulkanDevice&&) = delete;
};

} // namespace gfx

#endif // VULKANDEVICE_HPP
