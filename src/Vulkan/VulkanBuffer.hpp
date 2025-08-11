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

#include "Vulkan/vk_mem_alloc.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <cstddef>
    #include <vector>
    namespace ext = std;
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

    const vk::Buffer& vkBuffer() const;

    const vk::DescriptorBufferInfo& descriptorInfo() const;

    ~VulkanBuffer() override;

protected:
    void* contentVoid() override;

private:
    const VulkanDevice* m_device;
    const size_t m_size;

    struct FrameData
    {
        vk::Buffer vkBuffer;
        VmaAllocation allocation;
        VmaAllocationInfo allocInfo;
        vk::DescriptorBufferInfo descriptorInfo;
    };

    ext::vector<FrameData> m_frameDatas;

public:
    VulkanBuffer& operator=(const VulkanBuffer&) = delete;
    VulkanBuffer& operator=(VulkanBuffer&&) = delete;
};

} // namespace gfx

#endif // VULKANBUFFER_HPP
