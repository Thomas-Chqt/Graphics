/*
 * ---------------------------------------------------
 * VulkanParameterBlockLayout.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/11/11 07:50:29
 * ---------------------------------------------------
 */

#include "Vulkan/VulkanParameterBlockLayout.hpp"
#include "Graphics/Enums.hpp"
#include "Vulkan/VulkanEnums.hpp"
#include "Vulkan/VulkanDevice.hpp"

#include <algorithm>
#include <cassert>

namespace gfx
{

VulkanParameterBlockLayout::VulkanParameterBlockLayout(const VulkanDevice* device, const ParameterBlockLayout::Descriptor& desc)
    : m_device(device), m_bindings(desc.bindings)
{
    assert(m_device);

    std::vector<vk::DescriptorSetLayoutBinding> vkBindings;
    std::vector<vk::DescriptorBindingFlags> bindingFlags;
    vkBindings.reserve(desc.bindings.size());
    bindingFlags.reserve(desc.bindings.size());

    for (uint32_t i = 0; const auto& binding : desc.bindings) {
        assert(binding.count > 0);
        assert(binding.type == BindingType::sampledTexture || binding.count == 1);

        vkBindings.push_back(vk::DescriptorSetLayoutBinding{}
            .setBinding(i++)
            .setDescriptorType(toVkDescriptorType(binding.type))
            .setDescriptorCount(binding.count)
            .setStageFlags(toVkShaderStageFlags(binding.usages)));

        if (binding.count > 1)
            bindingFlags.push_back(vk::DescriptorBindingFlagBits::eUpdateAfterBind | vk::DescriptorBindingFlagBits::eUpdateUnusedWhilePending | vk::DescriptorBindingFlagBits::ePartiallyBound);
        else
            bindingFlags.emplace_back();
    }

    auto bindingFlagsCreateInfo = vk::DescriptorSetLayoutBindingFlagsCreateInfo{}
        .setBindingFlags(bindingFlags);

    auto descriptorSetLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo{}
        .setPNext(&bindingFlagsCreateInfo)
        .setBindings(vkBindings);

    if (std::ranges::any_of(bindingFlags, [](const vk::DescriptorBindingFlags& e) -> bool { return static_cast<bool>(e);}))
        descriptorSetLayoutCreateInfo.setFlags(vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool);

    m_vkDescriptorSetLayout = m_device->vkDevice().createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
}

VulkanParameterBlockLayout::~VulkanParameterBlockLayout()
{
    m_device->vkDevice().destroyDescriptorSetLayout(m_vkDescriptorSetLayout);
}

} // namespace gfx
