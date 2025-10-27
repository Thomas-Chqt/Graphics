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
#include "Vulkan/VulkanParameterBlockPool.hpp"

namespace gfx
{

VulkanParameterBlock::VulkanParameterBlock(const VulkanDevice* device, const std::shared_ptr<vk::DescriptorPool>& descriptorPool, const ParameterBlock::Layout& layout, VulkanParameterBlockPool* sourcePool)
    : m_device(device),
      m_descriptorPool(descriptorPool),
      m_layout(layout),
      m_sourcePool(sourcePool)
{
    auto descriptorSetAllocateInfo = vk::DescriptorSetAllocateInfo{}
        .setDescriptorPool(*m_descriptorPool)
        .setDescriptorSetCount(1)
        .setSetLayouts(m_device->descriptorSetLayout(layout)); // need to be in cache

    std::vector<vk::DescriptorSet> descriptorSets = m_device->vkDevice().allocateDescriptorSets(descriptorSetAllocateInfo);
    if (descriptorSets.empty())
        throw std::runtime_error("failed to allocate descriptorSet");

    m_descriptorSet = std::move(descriptorSets.front());
}

void VulkanParameterBlock::setBinding(uint32_t idx, const std::shared_ptr<Buffer>& aBuffer)
{
    auto buffer = std::dynamic_pointer_cast<VulkanBuffer>(aBuffer);

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

    m_usedBuffers.insert(std::make_pair(buffer, m_layout.bindings.at(idx)));
}

void VulkanParameterBlock::setBinding(uint32_t idx, const std::shared_ptr<Texture>& aTexture)
{
    auto texture = std::dynamic_pointer_cast<VulkanTexture>(aTexture);

    auto descriptorImageInfo = vk::DescriptorImageInfo{}
        .setImageView(texture->vkImageView())
        .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

    auto writeDescriptorSet = vk::WriteDescriptorSet{}
        .setDstSet(m_descriptorSet)
        .setDstBinding(idx)
        .setDstArrayElement(0)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eSampledImage)
        .setImageInfo(descriptorImageInfo);

    m_device->vkDevice().updateDescriptorSets(writeDescriptorSet, {});

    m_usedTextures.insert(std::make_pair(texture, m_layout.bindings.at(idx)));
}

void VulkanParameterBlock::setBinding(uint32_t idx, const std::shared_ptr<Sampler>& aSampler)
{
    auto sampler = std::dynamic_pointer_cast<VulkanSampler>(aSampler);

    auto descriptorImageInfo = vk::DescriptorImageInfo{}
        .setSampler(sampler->vkSampler());

    auto writeDescriptorSet = vk::WriteDescriptorSet{}
        .setDstSet(m_descriptorSet)
        .setDstBinding(idx)
        .setDstArrayElement(0)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eSampler)
        .setImageInfo(descriptorImageInfo);

    m_device->vkDevice().updateDescriptorSets(writeDescriptorSet, {});

    m_usedSampler.insert(std::make_pair(sampler, m_layout.bindings.at(idx)));
}

VulkanParameterBlock::~VulkanParameterBlock()
{
    if (m_sourcePool != nullptr)
        m_sourcePool->release(this);
}

}
