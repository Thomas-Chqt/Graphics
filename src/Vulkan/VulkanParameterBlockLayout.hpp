/*
 * ---------------------------------------------------
 * VulkanParameterBlockLayout.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/11/11 07:36:01
 * ---------------------------------------------------
 */

#ifndef VULKANPARAMETERBLOCKLAYOUT_HPP
#define VULKANPARAMETERBLOCKLAYOUT_HPP

#include "Graphics/ParameterBlockLayout.hpp"

#include "Vulkan/VulkanDevice.hpp"

namespace gfx
{

class VulkanParameterBlockLayout : public ParameterBlockLayout
{
public:
    VulkanParameterBlockLayout() = default;
    VulkanParameterBlockLayout(const VulkanParameterBlockLayout&) = delete;
    VulkanParameterBlockLayout(VulkanParameterBlockLayout&&) = delete;

    VulkanParameterBlockLayout(const VulkanDevice*, const ParameterBlockLayout::Descriptor&);

    ~VulkanParameterBlockLayout() override;

private:
    const VulkanDevice* m_device;
    vk::DescriptorSetLayout m_vkDescriptorSetLayout;

public:
    VulkanParameterBlockLayout& operator=(const VulkanParameterBlockLayout&) = delete;
    VulkanParameterBlockLayout& operator=(VulkanParameterBlockLayout&&) = delete;
};

} // namespace gfx

#endif // VULKANPARAMETERBLOCKLAYOUT_HPP
