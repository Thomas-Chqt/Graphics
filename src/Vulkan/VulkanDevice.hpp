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
#include "Graphics/ShaderLib.hpp"
#include "Graphics/GraphicsPipeline.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/CommandBufferPool.hpp"
#include "Graphics/ParameterBlockPool.hpp"
#include "Graphics/CommandBuffer.hpp"
#include "Graphics/Enums.hpp"

#include "Vulkan/QueueFamily.hpp"
#include "Vulkan/VulkanCommandBuffer.hpp"
#include <cstdint>
#include <memory>

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

    inline Backend backend() const override { return Backend::vulkan; }

    ext::unique_ptr<Swapchain> newSwapchain(const Swapchain::Descriptor&) const override;
    ext::unique_ptr<ShaderLib> newShaderLib(const ext::filesystem::path&) const override;
    ext::unique_ptr<GraphicsPipeline> newGraphicsPipeline(const GraphicsPipeline::Descriptor&) override; // need non cont to cache descriptorSetLayouts
    ext::unique_ptr<Buffer> newBuffer(const Buffer::Descriptor&) const override;
    ext::unique_ptr<Texture> newTexture(const Texture::Descriptor&) const override;
    ext::unique_ptr<CommandBufferPool> newCommandBufferPool() const override;
    ext::unique_ptr<ParameterBlockPool> newParameterBlockPool() const override;

#if defined (GFX_IMGUI_ENABLED)
    void imguiInit(ext::vector<PixelFormat> colorAttachmentPxFormats, ext::optional<PixelFormat> depthAttachmentPxFormat) const override;
    void imguiNewFrame() const override;
    void imguiShutdown() override;
#endif

    void submitCommandBuffers(ext::unique_ptr<CommandBuffer>&&) override;
    void submitCommandBuffers(ext::vector<ext::unique_ptr<CommandBuffer>>) override;

    void waitCommandBuffer(const CommandBuffer*) override;
    void waitIdle() override;

    inline const vk::Device& vkDevice() const { return m_vkDevice; }
    inline const VulkanPhysicalDevice& physicalDevice() const { return *m_physicalDevice; }

    inline const VmaAllocator& allocator() const { return m_allocator; }

    const vk::DescriptorSetLayout& descriptorSetLayout(const ParameterBlock::Layout&); // create one if not in cache
    inline const vk::DescriptorSetLayout& descriptorSetLayout(const ParameterBlock::Layout& pbl) const { return m_descriptorSetLayoutCache.at(pbl); } // thow if not in cache

    ~VulkanDevice() override;

private:
    const VulkanInstance* const m_instance = nullptr;
    const VulkanPhysicalDevice* const m_physicalDevice = nullptr;

    QueueFamily m_queueFamily;
    vk::Device m_vkDevice;
    vk::Queue m_queue;
    VmaAllocator m_allocator = VK_NULL_HANDLE;
    vk::Semaphore m_timelineSemaphore;
    ext::unique_ptr<VulkanCommandBufferPool> m_barrierCmdBufferPool;

    ext::map<ParameterBlock::Layout, vk::DescriptorSetLayout> m_descriptorSetLayoutCache;
    ext::deque<ext::unique_ptr<VulkanCommandBuffer>> m_submittedCommandBuffers;
    uint64_t m_nextSignaledTimeValue = 1;

public:
    VulkanDevice& operator=(const VulkanDevice&) = delete;
    VulkanDevice& operator=(VulkanDevice&&) = delete;
};

} // namespace gfx

#endif // VULKANDEVICE_HPP
