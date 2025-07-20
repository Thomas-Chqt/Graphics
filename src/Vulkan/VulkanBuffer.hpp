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

#include <vulkan/vulkan.hpp>

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

    const vk::Buffer& vkBuffer() const { return m_vkBuffer; }

    ~VulkanBuffer() override;

private:
    const VulkanDevice* m_device;

    vk::Buffer m_vkBuffer;
    vk::DeviceMemory m_memory;

public:
    VulkanBuffer& operator=(const VulkanBuffer&) = delete;
    VulkanBuffer& operator=(VulkanBuffer&&) = delete;
};

} // namespace gfx

#endif // VULKANBUFFER_HPP
