/*
 * ---------------------------------------------------
 * VulkanBuffer.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/07/14 06:53:53
 * ---------------------------------------------------
 */

#include "Graphics/Buffer.hpp"

#include "Vulkan/VulkanEnums.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanBuffer.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <algorithm>
    #include <cstddef>
    namespace ext = std;
#endif

namespace gfx
{
    
VulkanBuffer::VulkanBuffer(const VulkanDevice* device, const Buffer::Descriptor& desc)
    : m_device(device)
{
    auto bufferCreateInfo = vk::BufferCreateInfo{}
        .setSize(static_cast<vk::DeviceSize>(desc.size))
        .setUsage(toVkBufferUsageFlags(desc.usage))
        .setSharingMode(vk::SharingMode::eExclusive);
    m_vkBuffer = m_device->vkDevice().createBuffer(bufferCreateInfo);

    vk::MemoryRequirements memRequirements = m_device->vkDevice().getBufferMemoryRequirements(m_vkBuffer);
    uint32_t memoryTypeIdx = m_device->physicalDevice().findSuitableMemoryTypeIdx(
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        memRequirements.memoryTypeBits);

    auto memoryAllocateInfo = vk::MemoryAllocateInfo{}
        .setAllocationSize(memRequirements.size)
        .setMemoryTypeIndex(memoryTypeIdx);

    m_memory = m_device->vkDevice().allocateMemory(memoryAllocateInfo);

    m_device->vkDevice().bindBufferMemory(m_vkBuffer, m_memory, 0);

    if (desc.data)
    {
        void* bufferData = m_device->vkDevice().mapMemory(m_memory, 0, static_cast<vk::DeviceSize>(desc.size));
        ext::copy((ext::byte*)desc.data, (ext::byte*)desc.data + desc.size, (ext::byte*)bufferData);
        m_device->vkDevice().unmapMemory(m_memory);
    }
}

VulkanBuffer::~VulkanBuffer()
{
    m_device->vkDevice().freeMemory(m_memory);
    m_device->vkDevice().destroyBuffer(m_vkBuffer);
}

}
