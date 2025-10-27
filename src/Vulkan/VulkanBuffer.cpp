/*
 * ---------------------------------------------------
 * VulkanBuffer.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/07/14 06:53:53
 * ---------------------------------------------------
 */

#include "Graphics/Buffer.hpp"
#include "Graphics/Enums.hpp"

#include "Vulkan/VulkanEnums.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanBuffer.hpp"

namespace gfx
{

VulkanBuffer::VulkanBuffer(const VulkanDevice* device, const Buffer::Descriptor& desc)
    : m_device(device), m_size(desc.size), m_usages(desc.usages), m_storageMode(desc.storageMode)
{
    VkBufferCreateInfo bufferCreateInfo = vk::BufferCreateInfo{}
        .setSize(static_cast<vk::DeviceSize>(desc.size))
        .setUsage(toVkBufferUsageFlags(desc.usages))
        .setSharingMode(vk::SharingMode::eExclusive);

    VmaAllocationCreateInfo allocInfo = { .usage = VMA_MEMORY_USAGE_AUTO, };
    if (m_storageMode == ResourceStorageMode::hostVisible)
        allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VkBuffer buffer = VK_NULL_HANDLE;
    vmaCreateBuffer(m_device->allocator(), &bufferCreateInfo, &allocInfo, &buffer, &m_allocation, &m_allocInfo);
    m_vkBuffer = std::exchange(buffer, VK_NULL_HANDLE);
}

void VulkanBuffer::setContent(const void* data, size_t size)
{
    assert(m_storageMode == ResourceStorageMode::hostVisible);

    VkResult res = vmaCopyMemoryToAllocation(m_device->allocator(), data, m_allocation, 0, size);
    if (res != VK_SUCCESS)
        throw std::runtime_error("vmaCopyMemoryToAllocation failed");
}

VulkanBuffer::~VulkanBuffer()
{
    vmaDestroyBuffer(m_device->allocator(), m_vkBuffer, m_allocation);
}

void* VulkanBuffer::contentVoid()
{
    assert(m_storageMode == ResourceStorageMode::hostVisible);
    return m_allocInfo.pMappedData;
}

}
