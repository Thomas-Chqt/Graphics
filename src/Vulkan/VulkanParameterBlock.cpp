/*
 * ---------------------------------------------------
 * VulkanParameterBlock.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/10 11:06:42
 * ---------------------------------------------------
 */

#include "Graphics/Buffer.hpp"

#include "Vulkan/VulkanParameterBlock.hpp"
#include "Vulkan/VulkanBuffer.hpp"
#include "Vulkan/VulkanDevice.hpp"

namespace gfx
{

VulkanParameterBlock::VulkanParameterBlock(const VulkanDevice* device, vk::DescriptorSet&& descriptorSet)
    : m_device(device), m_descriptorSet(ext::move(descriptorSet))
{
}

void VulkanParameterBlock::setBinding(uint32_t idx, const ext::shared_ptr<const Buffer>& aBuffer)
{
    auto buffer = ext::dynamic_pointer_cast<const VulkanBuffer>(aBuffer);

    auto writeDescriptorSet = vk::WriteDescriptorSet{}
        .setDstSet(m_descriptorSet)
        .setDstBinding(idx)
        .setDstArrayElement(0)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setBufferInfo(buffer->descriptorInfo());

    m_device->vkDevice().updateDescriptorSets(writeDescriptorSet, {});

    m_usedBuffers.insert(buffer);
}

}
