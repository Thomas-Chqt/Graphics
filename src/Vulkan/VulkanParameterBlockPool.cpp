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

VulkanParameterBlockPool::VulkanParameterBlockPool(VulkanParameterBlockPool&& other) noexcept
    : m_device(ext::exchange(other.m_device, nullptr)),
      m_pools(ext::move(other.m_pools))
{
    other.m_frontPool = &other.m_pools[0];
    other.m_backPool = &other.m_pools[1];
}

VulkanParameterBlockPool::VulkanParameterBlockPool(const VulkanDevice* device)
    : m_device(device)
{
    ext::array<vk::DescriptorPoolSize, 1> poolSizes = {
        vk::DescriptorPoolSize{.type=vk::DescriptorType::eUniformBuffer, .descriptorCount=10}
    };

    auto descriptorPoolCreateInfo = vk::DescriptorPoolCreateInfo{}
        .setMaxSets(10)
        .setPoolSizes(poolSizes);

    m_pools[0].descriptorPool = m_device->vkDevice().createDescriptorPool(descriptorPoolCreateInfo);
    m_pools[1].descriptorPool = m_device->vkDevice().createDescriptorPool(descriptorPoolCreateInfo);
}

VulkanParameterBlock& VulkanParameterBlockPool::get(const ParameterBlock::Layout& pbLayout)
{
    auto descriptorSetAllocateInfo = vk::DescriptorSetAllocateInfo{}
        .setDescriptorPool(m_frontPool->descriptorPool)
        .setDescriptorSetCount(1)
        .setSetLayouts(m_device->descriptorSetLayout(pbLayout));
    ext::vector<vk::DescriptorSet> descriptorSets = m_device->vkDevice().allocateDescriptorSets(descriptorSetAllocateInfo);
    if (descriptorSets.empty())
        throw ext::runtime_error("failed to allocate descriptorSet");
    return m_frontPool->usedPBlocks.emplace_back(m_device, ext::move(descriptorSets.front()), pbLayout);
}

void VulkanParameterBlockPool::swapPools()
{
    ext::swap(m_frontPool, m_backPool);
}

void VulkanParameterBlockPool::reset()
{
    assert(m_device);
    m_backPool->usedPBlocks.clear();
    m_device->vkDevice().resetDescriptorPool(m_backPool->descriptorPool);
}

void VulkanParameterBlockPool::clear()
{
    for (auto& pool : m_pools)
    {
        pool.usedPBlocks.clear();
        if (pool.descriptorPool) {
            assert(m_device);
            m_device->vkDevice().destroyDescriptorPool(pool.descriptorPool);
            pool.descriptorPool = VK_NULL_HANDLE;
        }
    }
    m_device = nullptr;
}

VulkanParameterBlockPool::~VulkanParameterBlockPool()
{
    clear();
}

VulkanParameterBlockPool& VulkanParameterBlockPool::operator=(VulkanParameterBlockPool&& other) noexcept
{
    if (this != &other)
    {
        clear();
        m_device = ext::exchange(other.m_device, nullptr);
        m_pools = ext::move(other.m_pools);

        other.m_frontPool = &other.m_pools[0];
        other.m_backPool = &other.m_pools[1];
    }
    return *this;
}

}
