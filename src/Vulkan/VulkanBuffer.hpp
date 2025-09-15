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
#include "Graphics/Enums.hpp"

#include "Vulkan/Sync.hpp"

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
    inline BufferUsages usages() const override { return m_usages; };
    inline ResourceStorageMode storageMode() const override { return m_storageMode; };

    void setContent(const void* data, size_t size) override;

    inline const vk::Buffer& vkBuffer() const { return m_vkBuffer; }
    inline BufferSyncState& syncState() { return m_syncState; }
    inline const BufferSyncState& syncState() const { return m_syncState; }

    ~VulkanBuffer() override;

protected:
    void* contentVoid() override;

private:
    const VulkanDevice* m_device;
    const size_t m_size;
    BufferUsages m_usages;
    ResourceStorageMode m_storageMode;

    vk::Buffer m_vkBuffer;
    VmaAllocation m_allocation = VK_NULL_HANDLE;
    VmaAllocationInfo m_allocInfo = {};

    BufferSyncState m_syncState;

public:
    VulkanBuffer& operator=(const VulkanBuffer&) = delete;
    VulkanBuffer& operator=(VulkanBuffer&&) = delete;
};

} // namespace gfx

#endif // VULKANBUFFER_HPP
