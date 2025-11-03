/*
 * ---------------------------------------------------
 * VulkanParameterBlockPool.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/10 17:53:12
 * ---------------------------------------------------
 */

#include "Graphics/ParameterBlock.hpp"

#include "Vulkan/VulkanParameterBlockPool.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanParameterBlock.hpp"

#include <algorithm>

namespace gfx
{

VulkanParameterBlockPool::VulkanParameterBlockPool(const VulkanDevice* device, const ParameterBlockPool::Descriptor& descriptor)
    : m_device(device)
{
    std::array<vk::DescriptorPoolSize, 3> poolSizes = {
        vk::DescriptorPoolSize{.type=vk::DescriptorType::eUniformBuffer, .descriptorCount=descriptor.maxUniformBuffers},
        vk::DescriptorPoolSize{.type=vk::DescriptorType::eSampledImage, .descriptorCount=descriptor.maxTextures},
        vk::DescriptorPoolSize{.type=vk::DescriptorType::eSampler, .descriptorCount=descriptor.maxSamplers}
    };

    uint32_t maxSets = std::max({descriptor.maxUniformBuffers, descriptor.maxTextures, descriptor.maxSamplers});
    if (maxSets < 1) maxSets = 1;

    auto descriptorPoolCreateInfo = vk::DescriptorPoolCreateInfo{}
        .setMaxSets(maxSets)
        .setPoolSizes(poolSizes);

    m_descriptorPool = std::shared_ptr<vk::DescriptorPool>(
        new vk::DescriptorPool(m_device->vkDevice().createDescriptorPool(descriptorPoolCreateInfo)),
        [device=m_device](vk::DescriptorPool* pool){
            device->vkDevice().destroyDescriptorPool(*pool);
            delete pool; // NOLINT(cppcoreguidelines-owning-memory)
        }
    );
}

std::unique_ptr<ParameterBlock> VulkanParameterBlockPool::get(const ParameterBlock::Layout& pbLayout)
{
    auto pBlock = std::make_unique<VulkanParameterBlock>(m_device, m_descriptorPool, pbLayout, this);
    m_usedParameterBlocks.insert(pBlock.get());
    return pBlock;
}

void VulkanParameterBlockPool::release(ParameterBlock* aPBlock)
{
    auto* pBlock = dynamic_cast<VulkanParameterBlock*>(aPBlock);
    assert(m_usedParameterBlocks.contains(pBlock));
    m_usedParameterBlocks.erase(pBlock);
    if (m_usedParameterBlocks.empty())
        m_device->vkDevice().resetDescriptorPool(*m_descriptorPool);
}

VulkanParameterBlockPool::~VulkanParameterBlockPool()
{
    for (auto* pBlock : m_usedParameterBlocks) {
        // blocks can outlive the pool, this prevent releasing a block to a freed pool
        pBlock->clearSourcePool();
    }
}

}
