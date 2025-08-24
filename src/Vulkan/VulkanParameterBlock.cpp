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

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <cstdint>
    #include <memory>
    namespace ext = std;
#endif

namespace gfx
{

VulkanParameterBlock::VulkanParameterBlock(const VulkanDevice* device, vk::DescriptorSet&& descriptorSet, const ParameterBlock::Layout& layout)
    : m_device(device),
      m_descriptorSet(ext::move(descriptorSet)),
      m_bindings(layout.bindings)
{
}

void VulkanParameterBlock::setBinding(uint32_t idx, const ext::shared_ptr<Buffer>& aBuffer)
{
    auto buffer = ext::dynamic_pointer_cast<VulkanBuffer>(aBuffer);

    auto bufferDescriptorInfo = vk::DescriptorBufferInfo{}
        .setBuffer(buffer->vkBuffer())
        .setOffset(0)
        .setRange(vk::WholeSize);

    auto writeDescriptorSet = vk::WriteDescriptorSet{}
        .setDstSet(m_descriptorSet)
        .setDstBinding(idx)
        .setDstArrayElement(0)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setBufferInfo(bufferDescriptorInfo);

    m_device->vkDevice().updateDescriptorSets(writeDescriptorSet, {});

    m_usedBuffers.insert(ext::make_pair(m_bindings.at(idx), buffer));
}

}
