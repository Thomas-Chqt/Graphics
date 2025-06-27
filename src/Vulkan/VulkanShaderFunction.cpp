/*
 * ---------------------------------------------------
 * VulkanShaderFunction.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/27 12:52:42
 * ---------------------------------------------------
 */

#include "Vulkan/VulkanShaderFunction.hpp"

namespace gfx
{

VulkanShaderFunction::VulkanShaderFunction(const vk::ShaderModule* shaderModule, const ext::string& name)
    : m_shaderModule(shaderModule), m_name(name)
{
}

}
