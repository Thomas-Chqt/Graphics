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
#include <vulkan/vulkan_structs.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <cstddef>
    #include <stdexcept>
    namespace ext = std;
#endif

namespace gfx
{
    
VulkanBuffer::VulkanBuffer(const VulkanDevice* device, const Buffer::Descriptor& desc)
    : m_device(device), m_size(desc.size)
{
    VkBufferCreateInfo bufferCreateInfo = vk::BufferCreateInfo{}
        .setSize(static_cast<vk::DeviceSize>(desc.size))
        .setUsage(toVkBufferUsageFlags(desc.usage))
        .setSharingMode(vk::SharingMode::eExclusive);

    VmaAllocationCreateInfo allocInfo = { .usage = VMA_MEMORY_USAGE_AUTO, };
    if (desc.storageMode == ResourceStorageMode::hostVisible)
        allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    m_frameDatas.resize(desc.usage & BufferUsage::perFrameData ? m_device->maxFrameInFlight() : 1);
    for (auto& frameData : m_frameDatas)
    {
        VkBuffer buffer;
        vmaCreateBuffer(m_device->allocator(), &bufferCreateInfo, &allocInfo, &buffer, &frameData.allocation, &frameData.allocInfo);
        frameData.vkBuffer = std::move(buffer);
        frameData.descriptorInfo = vk::DescriptorBufferInfo{}
            .setBuffer(frameData.vkBuffer)
            .setOffset(0)
            .setRange(vk::WholeSize);
    }
    if (desc.data)
    {
        assert(desc.storageMode == ResourceStorageMode::hostVisible);
        setContent(desc.data, desc.size);
    }
}

void VulkanBuffer::setContent(const void* data, size_t size)
{
    for (auto& frameData : m_frameDatas)
    {
        VkResult res = vmaCopyMemoryToAllocation(m_device->allocator(), data, frameData.allocation, 0, size);
        if (res != VK_SUCCESS)
            throw ext::runtime_error("vmaCopyMemoryToAllocation failed");
    }
}

const vk::Buffer& VulkanBuffer::vkBuffer() const
{
    return m_frameDatas[m_device->currentFrameIdx() % m_frameDatas.size()].vkBuffer;
}

const vk::DescriptorBufferInfo& VulkanBuffer::descriptorInfo() const
{
    return m_frameDatas[m_device->currentFrameIdx() % m_frameDatas.size()].descriptorInfo;
}

VulkanBuffer::~VulkanBuffer()
{
    for (auto& frameData : m_frameDatas)
    {
        vmaDestroyBuffer(m_device->allocator(), frameData.vkBuffer, frameData.allocation);
    }
}

void* VulkanBuffer::contentVoid()
{
    return m_frameDatas[m_device->currentFrameIdx() % m_frameDatas.size()].allocInfo.pMappedData;
}

}
