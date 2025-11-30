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

std::shared_ptr<ParameterBlock> VulkanParameterBlockPool::get(const std::shared_ptr<ParameterBlockLayout>& aPbLayout)
{
    auto pbLayout = std::dynamic_pointer_cast<VulkanParameterBlockLayout>(aPbLayout);
    assert(pbLayout);
    std::shared_ptr<VulkanParameterBlock> pBlock;
    if (m_availablePBlocks.empty() == false) {
        pBlock = std::move(m_availablePBlocks.front());
        m_availablePBlocks.pop_front();
        *pBlock = VulkanParameterBlock(m_device, pbLayout, m_descriptorPool);
    }
    else {
        pBlock = std::make_shared<VulkanParameterBlock>(m_device, pbLayout, m_descriptorPool);
    }
    m_usedPBlocks.push_back(pBlock);
    return pBlock;
}

void VulkanParameterBlockPool::reset()
{
    m_device->vkDevice().resetDescriptorPool(*m_descriptorPool);
    for (auto& pBlock : m_usedPBlocks) {
        *pBlock = VulkanParameterBlock();
        m_availablePBlocks.push_back(std::move(pBlock));
    }
    m_usedPBlocks.clear();
}

}
