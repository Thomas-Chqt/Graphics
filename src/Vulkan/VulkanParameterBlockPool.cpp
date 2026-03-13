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
#include "Vulkan/VulkanEnums.hpp"
#include "Vulkan/VulkanParameterBlock.hpp"
#include "Vulkan/VulkanParameterBlockLayout.hpp"

namespace gfx
{

VulkanParameterBlockPool::VulkanParameterBlockPool(const VulkanDevice* device, const ParameterBlockPool::Descriptor& descriptor)
    : m_device(device)
{
    std::vector<vk::DescriptorPoolSize> poolSizes(descriptor.maxBindingCount.size());
    for (uint32_t i = 0; auto [key, val] : descriptor.maxBindingCount) {
        assert(i<descriptor.maxBindingCount.size());
        poolSizes[i++] = vk::DescriptorPoolSize{
            .type = toVkDescriptorType(key),
            .descriptorCount = val
        };
    }

    auto descriptorPoolCreateInfo = vk::DescriptorPoolCreateInfo{}
        .setMaxSets(std::accumulate(descriptor.maxBindingCount.begin(), descriptor.maxBindingCount.end(), 0, [](auto acc, const auto& kv) { return acc + kv.second; }))
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
