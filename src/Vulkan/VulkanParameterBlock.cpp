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
#include "Vulkan/VulkanEnums.hpp"
#include "Vulkan/VulkanParameterBlockPool.hpp"
#include "Vulkan/VulkanParameterBlockLayout.hpp"

namespace gfx
{

VulkanParameterBlock::VulkanParameterBlock(const VulkanDevice* device, const std::shared_ptr<VulkanParameterBlockLayout>& layout, const std::shared_ptr<vk::DescriptorPool>& descriptorPool)
    : m_device(device),
      m_layout(layout),
      m_descriptorPool(descriptorPool)
{
    assert(m_device);
    assert(m_layout);
    assert(m_descriptorPool);

    m_usedBuffers.resize(m_layout->bindings().size());
    m_usedTextures.resize(m_layout->bindings().size());
    m_usedSamplers.resize(m_layout->bindings().size());

    auto descriptorSetAllocateInfo = vk::DescriptorSetAllocateInfo{}
        .setDescriptorPool(*m_descriptorPool)
        .setDescriptorSetCount(1)
        .setSetLayouts(m_layout->vkDescriptorSetLayout());

    try {
        std::vector<vk::DescriptorSet> descriptorSets = m_device->vkDevice().allocateDescriptorSets(descriptorSetAllocateInfo);
        m_descriptorSet = std::move(descriptorSets.front());
    } catch (...) {
        throw std::runtime_error("failed to allocate descriptorSet");
    }
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
        .setDescriptorType(toVkDescriptorType(m_layout->bindings()[idx].type))
        .setBufferInfo(bufferDescriptorInfo);

    m_device->vkDevice().updateDescriptorSets(writeDescriptorSet, {});
    auto& usedBuffers = m_usedBuffers.at(idx);
    usedBuffers.insert_or_assign(0, UsedResource<VulkanBuffer>{
        .resource = buffer,
        .binding = m_layout->bindings().at(idx)
    });
}

void VulkanParameterBlock::setBinding(uint32_t idx, const std::shared_ptr<Texture>& aTexture)
{
    setBinding(idx, 0, aTexture);
}

void VulkanParameterBlock::setBinding(uint32_t idx, uint32_t arrayIndex, const std::shared_ptr<Texture>& aTexture)
{
    setBinding(idx, arrayIndex, std::span(&aTexture, 1));
}

void VulkanParameterBlock::setBinding(uint32_t idx, uint32_t firstArrayIndex, std::span<const std::shared_ptr<Texture>> textures)
{
    assert(m_layout->bindings().at(idx).type == BindingType::sampledTexture);
    assert(firstArrayIndex + textures.size() <= m_layout->bindings().at(idx).count);

    std::vector<vk::DescriptorImageInfo> descriptorImageInfos;
    descriptorImageInfos.reserve(textures.size());
    auto& usedTextures = m_usedTextures.at(idx);

    for (uint32_t i = 0; const auto& texturePtr : textures) {
        auto texture = std::dynamic_pointer_cast<VulkanTexture>(texturePtr);
        assert(texture);

        descriptorImageInfos.push_back(vk::DescriptorImageInfo{}
            .setImageView(texture->vkImageView())
            .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal));
        usedTextures.insert_or_assign(firstArrayIndex + i, UsedResource<VulkanTexture>{
            .resource = texture,
            .binding = m_layout->bindings().at(idx)
        });
        ++i;
    }

    auto writeDescriptorSet = vk::WriteDescriptorSet{}
        .setDstSet(m_descriptorSet)
        .setDstBinding(idx)
        .setDstArrayElement(firstArrayIndex)
        .setDescriptorCount(static_cast<uint32_t>(descriptorImageInfos.size()))
        .setDescriptorType(vk::DescriptorType::eSampledImage)
        .setImageInfo(descriptorImageInfos);

    m_device->vkDevice().updateDescriptorSets(writeDescriptorSet, {});
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
    auto& usedSamplers = m_usedSamplers.at(idx);
    usedSamplers.insert_or_assign(0, UsedResource<VulkanSampler>{
        .resource = sampler,
        .binding = m_layout->bindings().at(idx)
    });
}

void VulkanParameterBlock::clearBinding(uint32_t idx, uint32_t arrayIndex)
{
    clearBinding(idx, arrayIndex, 1);
}

void VulkanParameterBlock::clearBinding(uint32_t idx, uint32_t firstArrayIndex, uint32_t count)
{
    assert(count > 0);
    assert(firstArrayIndex + count <= m_layout->bindings().at(idx).count);

    auto eraseBindingRange = [firstArrayIndex, count]<typename T>(std::unordered_map<uint32_t, UsedResource<T>>& resources) {
        std::erase_if(resources, [firstArrayIndex, count](const auto& entry) {
            return entry.first >= firstArrayIndex &&
                entry.first < firstArrayIndex + count;
        });
    };

    switch (m_layout->bindings().at(idx).type)
    {
    case BindingType::constantBuffer:
    case BindingType::structuredBuffer:
        eraseBindingRange(m_usedBuffers.at(idx));
        break;
    case BindingType::sampledTexture:
        eraseBindingRange(m_usedTextures.at(idx));
        break;
    case BindingType::sampler:
        eraseBindingRange(m_usedSamplers.at(idx));
        break;
    default:
        std::unreachable();
    }
}

} // namespace gfx
