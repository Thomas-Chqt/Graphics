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

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <utility>
    #include <stdexcept>
    namespace ext = std;
#endif

namespace gfx
{

VulkanParameterBlockPool::VulkanParameterBlockPool(const VulkanDevice* device)
    : m_device(device)
{
    ext::array<vk::DescriptorPoolSize, 1> poolSizes = {
        vk::DescriptorPoolSize{.type=vk::DescriptorType::eUniformBuffer, .descriptorCount=10}
    };

    auto descriptorPoolCreateInfo = vk::DescriptorPoolCreateInfo{}
        .setMaxSets(10)
        .setPoolSizes(poolSizes);

    m_descriptorPool = m_device->vkDevice().createDescriptorPool(descriptorPoolCreateInfo);
}

VulkanParameterBlock& VulkanParameterBlockPool::get(const ParameterBlock::Layout& pbLayout)
{
    auto descriptorSetAllocateInfo = vk::DescriptorSetAllocateInfo{}
        .setDescriptorPool(m_descriptorPool)
        .setDescriptorSetCount(1)
        .setSetLayouts(m_device->descriptorSetLayout(pbLayout));
    ext::vector<vk::DescriptorSet> descriptorSets = m_device->vkDevice().allocateDescriptorSets(descriptorSetAllocateInfo);
    if (descriptorSets.empty())
        throw ext::runtime_error("failed to allocate descriptorSet");
    return m_usedPBlocks.emplace_back(m_device, ext::move(descriptorSets.front()));
}

void VulkanParameterBlockPool::reset()
{
    m_usedPBlocks.clear();
    m_device->vkDevice().resetDescriptorPool(m_descriptorPool);
}

VulkanParameterBlockPool::~VulkanParameterBlockPool()
{
    if (m_descriptorPool)
        m_device->vkDevice().destroyDescriptorPool(m_descriptorPool);
}

VulkanParameterBlockPool& VulkanParameterBlockPool::operator=(VulkanParameterBlockPool&& other) noexcept
{
    if (this != &other)
    {
        m_usedPBlocks = ext::move(other.m_usedPBlocks);
        if (m_descriptorPool)
            m_device->vkDevice().destroyDescriptorPool(m_descriptorPool);
        m_descriptorPool = ext::move(other.m_descriptorPool);
        m_device = ext::exchange(other.m_device, nullptr);

    }
    return *this;
}

}
