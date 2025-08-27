/*
 * ---------------------------------------------------
 * VulkanShaderFunction.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/27 12:41:02
 * ---------------------------------------------------
 */

#ifndef VULKANSHADERFUNCTION_HPP
#define VULKANSHADERFUNCTION_HPP

#include "Graphics/ShaderFunction.hpp"

namespace gfx
{

class VulkanShaderFunction : public ShaderFunction
{
public:
    VulkanShaderFunction() = delete;
    VulkanShaderFunction(const VulkanShaderFunction&) = delete;
    VulkanShaderFunction(VulkanShaderFunction&&) = default;

    VulkanShaderFunction(const vk::ShaderModule*, const ext::string&);

    const vk::ShaderModule& shaderModule(void) const { return *m_shaderModule; }
    const ext::string& name(void) const { return m_name; }

    ~VulkanShaderFunction() = default;

private:
    const vk::ShaderModule* m_shaderModule;
    ext::string m_name;

public:
    VulkanShaderFunction& operator = (const VulkanShaderFunction&) = delete;
    VulkanShaderFunction& operator = (VulkanShaderFunction&&);
};

}

#endif // VULKANSHADERFUNCTION_HPP
