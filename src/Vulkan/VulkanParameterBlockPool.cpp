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
#include "VulkanParameterBlockLayout.hpp"

namespace gfx
{

VulkanParameterBlockPool::VulkanParameterBlockPool(const VulkanDevice* device, const ParameterBlockPool::Descriptor& descriptor)
    : m_device(device)
{
    std::array<vk::DescriptorPoolSize, 3> poolSizes = {
        vk::DescriptorPoolSize{.type=vk::DescriptorType::eUniformBuffer, .descriptorCount=descriptor.maxUniformBuffers > 0 ? descriptor.maxUniformBuffers : 1},
        vk::DescriptorPoolSize{.type=vk::DescriptorType::eSampledImage,  .descriptorCount=descriptor.maxTextures       > 0 ? descriptor.maxTextures       : 1},
        vk::DescriptorPoolSize{.type=vk::DescriptorType::eSampler,       .descriptorCount=descriptor.maxSamplers       > 0 ? descriptor.maxSamplers       : 1}
    };

    auto descriptorPoolCreateInfo = vk::DescriptorPoolCreateInfo{}
        .setMaxSets(descriptor.maxUniformBuffers + descriptor.maxTextures + descriptor.maxSamplers)
        .setPoolSizes(poolSizes);

    m_descriptorPool = std::shared_ptr<vk::DescriptorPool>(
        new vk::DescriptorPool(m_device->vkDevice().createDescriptorPool(descriptorPoolCreateInfo)),
        [device=m_device](vk::DescriptorPool* pool){
            device->vkDevice().destroyDescriptorPool(*pool);
            delete pool; // NOLINT(cppcoreguidelines-owning-memory)
        }
    );
}

std::unique_ptr<ParameterBlock> VulkanParameterBlockPool::get(const std::shared_ptr<ParameterBlockLayout>& aPbLayout)
{
    auto pbLayout = std::dynamic_pointer_cast<VulkanParameterBlockLayout>(aPbLayout);
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
