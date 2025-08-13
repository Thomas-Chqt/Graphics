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

#include "Graphics/Texture.hpp"
#include "common.hpp"
#include "Vulkan/QueueFamily.hpp"
#include "Vulkan/VulkanCommandBuffer.hpp"
#include "Vulkan/VulkanDrawable.hpp"
#include "Vulkan/VulkanParameterBlockPool.hpp"
#include "Vulkan/VulkanTexture.hpp"

#include "Vulkan/vk_mem_alloc.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    #include <memory>
    #include <cstddef>
    #include <cstdint>
    #include <iterator>
    #include <deque>
    #include <map>
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
    ext::unique_ptr<GraphicsPipeline> newGraphicsPipeline(const GraphicsPipeline::Descriptor&) override; // need non cont to cache descriptorSetLayouts
    ext::unique_ptr<Buffer> newBuffer(const Buffer::Descriptor&) const override;
    ext::unique_ptr<Texture> newTexture(const Texture::Descriptor&) const override;

#if defined(GFX_IMGUI_ENABLED)
    void imguiInit(uint32_t imageCount,
                   ext::vector<PixelFormat> colorAttachmentPxFormats,
                   ext::optional<PixelFormat> depthAttachmentPxFormat) const override;
#endif

#if defined(GFX_IMGUI_ENABLED)
    void imguiNewFrame() const override;
#endif

    void beginFrame() override;

    ParameterBlock& parameterBlock(const ParameterBlock::Layout&) override;

    CommandBuffer& commandBuffer() override;
 
    void submitCommandBuffer(CommandBuffer&) override;
    void presentDrawable(const ext::shared_ptr<Drawable>&) override;

    void endFrame() override;

    void waitIdle() const override;

#if defined(GFX_IMGUI_ENABLED)
    void imguiShutdown() const override;
#endif

    inline Backend backend() const override { return Backend::vulkan; }
    inline uint32_t currentFrameIdx() const override { return static_cast<uint32_t>(ext::distance(m_frameDatas.begin(), PerFrameInFlight<FrameData>::const_iterator(m_currFrameData))); };

    inline const vk::Device& vkDevice() const { return m_vkDevice; }
    inline const VulkanPhysicalDevice& physicalDevice() const { return *m_physicalDevice; }
    inline const VmaAllocator& allocator() const { return m_allocator; }

    const vk::DescriptorSetLayout& descriptorSetLayout(const ParameterBlock::Layout&);
    inline const vk::DescriptorSetLayout& descriptorSetLayout(const ParameterBlock::Layout& pbl) const { return m_descriptorSetLayouts.at(pbl); }

    ~VulkanDevice() override;

private:
    struct FrameData
    {
        VulkanParameterBlockPool pbPool;
        vk::CommandPool commandPool; // TODO : CommandBufferPool
        vk::Fence frameCompletedFence;
        ext::deque<VulkanCommandBuffer> commandBuffers;
        ext::deque<VulkanCommandBuffer> usedCommandBuffers;
        ext::vector<VulkanCommandBuffer*> submittedCmdBuffers;
        ext::vector<ext::shared_ptr<VulkanDrawable>> presentedDrawables;
    };
    
    const VulkanInstance* const m_instance = nullptr;
    const VulkanPhysicalDevice* const m_physicalDevice = nullptr;
    QueueFamily m_queueFamily;
    vk::Device m_vkDevice;
    vk::Queue m_queue;
    VmaAllocator m_allocator = VK_NULL_HANDLE;

    PerFrameInFlight<FrameData> m_frameDatas;
    PerFrameInFlight<FrameData>::iterator m_currFrameData = m_frameDatas.begin();

    ext::map<ParameterBlock::Layout, vk::DescriptorSetLayout> m_descriptorSetLayouts;

public:
    VulkanDevice& operator=(const VulkanDevice&) = delete;
    VulkanDevice& operator=(VulkanDevice&&) = delete;
};

} // namespace gfx

#endif // VULKANDEVICE_HPP
