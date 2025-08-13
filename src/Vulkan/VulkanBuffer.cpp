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
#include "Vulkan/vk_mem_alloc.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <cstddef>
    #include <stdexcept>
    #include <cassert>
    #include <utility>
    namespace ext = std;
#endif

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

    for (auto& frameData : m_frameDatas)
    {
        VkBuffer buffer = VK_NULL_HANDLE;
        vmaCreateBuffer(m_device->allocator(), &bufferCreateInfo, &allocInfo, &buffer, &frameData.allocation, &frameData.allocInfo);
        frameData.vkBuffer = ext::exchange(buffer, VK_NULL_HANDLE);
        frameData.descriptorInfo = vk::DescriptorBufferInfo{}
            .setBuffer(frameData.vkBuffer)
            .setOffset(0)
            .setRange(vk::WholeSize);

        if (static_cast<bool>(desc.usages & BufferUsage::perFrameData) == false)
            break;
    }
    if (desc.data)
        setContent(desc.data, desc.size);
}

void VulkanBuffer::setContent(const void* data, size_t size)
{
    assert(m_storageMode == ResourceStorageMode::hostVisible);
    for (auto& frameData : m_frameDatas)
    {
        VkResult res = vmaCopyMemoryToAllocation(m_device->allocator(), data, frameData.allocation, 0, size);
        if (res != VK_SUCCESS)
            throw ext::runtime_error("vmaCopyMemoryToAllocation failed");

        if (static_cast<bool>(m_usages & BufferUsage::perFrameData) == false)
            break;
    }
}

VulkanBuffer::~VulkanBuffer()
{
    for (auto& frameData : m_frameDatas)
    {
        vmaDestroyBuffer(m_device->allocator(), frameData.vkBuffer, frameData.allocation);

        if (static_cast<bool>(m_usages & BufferUsage::perFrameData) == false)
            break;
    }
}

void* VulkanBuffer::contentVoid()
{
    assert(m_storageMode == ResourceStorageMode::hostVisible);
    return currentFrameData().allocInfo.pMappedData;
}

VulkanBuffer::FrameData& VulkanBuffer::currentFrameData()
{
    uint64_t bufferCount = static_cast<bool>(m_usages & BufferUsage::perFrameData) ? m_frameDatas.size() : 1;
    return m_frameDatas.at(m_device->currentFrameIdx() % bufferCount);
}

const VulkanBuffer::FrameData& VulkanBuffer::currentFrameData() const
{
    uint64_t bufferCount = static_cast<bool>(m_usages & BufferUsage::perFrameData) ? m_frameDatas.size() : 1;
    return m_frameDatas.at(m_device->currentFrameIdx() % bufferCount);
}

}
