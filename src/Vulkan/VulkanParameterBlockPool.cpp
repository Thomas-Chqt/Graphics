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

namespace gfx
{

VulkanParameterBlockPool::VulkanParameterBlockPool(const VulkanDevice* device)
    : m_device(device)
{
    ext::array<vk::DescriptorPoolSize, 3> poolSizes = {
        vk::DescriptorPoolSize{.type=vk::DescriptorType::eUniformBuffer, .descriptorCount=1000},
        vk::DescriptorPoolSize{.type=vk::DescriptorType::eSampledImage, .descriptorCount=10},
        vk::DescriptorPoolSize{.type=vk::DescriptorType::eSampler, .descriptorCount=10}
    };

    auto descriptorPoolCreateInfo = vk::DescriptorPoolCreateInfo{}
        .setMaxSets(1000)
        .setPoolSizes(poolSizes);

    m_descriptorPool = ext::shared_ptr<vk::DescriptorPool>(
        new vk::DescriptorPool(m_device->vkDevice().createDescriptorPool(descriptorPoolCreateInfo)),
        [device=m_device](vk::DescriptorPool* pool){
            device->vkDevice().destroyDescriptorPool(*pool);
            delete pool; // NOLINT(cppcoreguidelines-owning-memory)
        }
    );
}

ext::unique_ptr<ParameterBlock> VulkanParameterBlockPool::get(const ParameterBlock::Layout& pbLayout)
{
    auto pBlock = ext::make_unique<VulkanParameterBlock>(m_device, m_descriptorPool, pbLayout, this);
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
