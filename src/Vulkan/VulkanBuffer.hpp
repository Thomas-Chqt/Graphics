/*
 * ---------------------------------------------------
 * VulkanBuffer.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/07/14 06:49:36
 * ---------------------------------------------------
 */

#ifndef VULKANBUFFER_HPP
#define VULKANBUFFER_HPP

#include "Graphics/Buffer.hpp"

#include "common.hpp"
#include "Vulkan/vk_mem_alloc.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <cstddef>
    namespace ext = std; // NOLINT
#endif

namespace gfx
{

class VulkanDevice;

class VulkanBuffer : public Buffer
{
public:
    VulkanBuffer() = delete;
    VulkanBuffer(const VulkanBuffer&) = delete;
    VulkanBuffer(VulkanBuffer&&) = delete;

    VulkanBuffer(const VulkanDevice*, const Buffer::Descriptor&);

    inline size_t size() const override { return m_size; }

    void setContent(const void* data, size_t size) override;

    inline const vk::Buffer& vkBuffer() const { return currentFrameData().vkBuffer; }
    inline const vk::DescriptorBufferInfo& descriptorInfo() const { return currentFrameData().descriptorInfo; }

    ~VulkanBuffer() override;

protected:
    void* contentVoid() override;

private:
    struct FrameData
    {
        vk::Buffer vkBuffer;
        VmaAllocation allocation;
        VmaAllocationInfo allocInfo;
        vk::DescriptorBufferInfo descriptorInfo;
    };

    FrameData& currentFrameData();
    const FrameData& currentFrameData() const;

    const VulkanDevice* m_device;
    const size_t m_size;
    BufferUsages m_usages;
    ResourceStorageMode m_storageMode;

    PerFrameInFlight<FrameData> m_frameDatas;

public:
    VulkanBuffer& operator=(const VulkanBuffer&) = delete;
    VulkanBuffer& operator=(VulkanBuffer&&) = delete;
};

} // namespace gfx

#endif // VULKANBUFFER_HPP
