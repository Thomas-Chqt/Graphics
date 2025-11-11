/*
 * ---------------------------------------------------
 * VulkanParameterBlockLayout.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/11/11 07:50:29
 * ---------------------------------------------------
 */

#include "Vulkan/VulkanParameterBlockLayout.hpp"
#include "Vulkan/VulkanEnums.hpp"
#include "Vulkan/VulkanDevice.hpp"

namespace gfx
{

VulkanParameterBlockLayout::VulkanParameterBlockLayout(const VulkanDevice* device, const ParameterBlockLayout::Descriptor& desc)
    : m_device(device)
{
    assert(m_device);

    std::vector<vk::DescriptorSetLayoutBinding> vkBindings;
    for (uint32_t i = 0; const auto& binding : desc.bindings) {
        vkBindings.push_back(vk::DescriptorSetLayoutBinding{}
            .setBinding(i)
            .setDescriptorType(toVkDescriptorType(binding.type))
            .setDescriptorCount(1)
            .setStageFlags(toVkShaderStageFlags(binding.usages)));
        i++;
    }
    auto descriptorSetLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo{}
        .setBindings(vkBindings);
    m_vkDescriptorSetLayout = m_device->vkDevice().createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
}

VulkanParameterBlockLayout::~VulkanParameterBlockLayout()
{
    m_device->vkDevice().destroyDescriptorSetLayout(m_vkDescriptorSetLayout);
}

}
