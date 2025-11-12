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

namespace gfx
{

class VulkanDevice;

class VulkanParameterBlockLayout : public ParameterBlockLayout
{
public:
    VulkanParameterBlockLayout() = delete;
    VulkanParameterBlockLayout(const VulkanParameterBlockLayout&) = delete;
    VulkanParameterBlockLayout(VulkanParameterBlockLayout&&) = delete;

    VulkanParameterBlockLayout(const VulkanDevice*, const ParameterBlockLayout::Descriptor&);

    inline const std::vector<ParameterBlockBinding>& bindings() const override { return m_bindings; };
    inline const vk::DescriptorSetLayout& vkDescriptorSetLayout() const { return m_vkDescriptorSetLayout; }

    ~VulkanParameterBlockLayout() override;

private:
    const VulkanDevice* m_device;
    std::vector<ParameterBlockBinding> m_bindings;
    vk::DescriptorSetLayout m_vkDescriptorSetLayout;

public:
    VulkanParameterBlockLayout& operator=(const VulkanParameterBlockLayout&) = delete;
    VulkanParameterBlockLayout& operator=(VulkanParameterBlockLayout&&) = delete;
};

} // namespace gfx

#endif // VULKANPARAMETERBLOCKLAYOUT_HPP
