/*
 * ---------------------------------------------------
 * VulkanShaderLib.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/26 10:33:14
 * ---------------------------------------------------
 */

#include "Vulkan/VulkanShaderLib.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanShaderFunction.hpp"

namespace gfx
{

VulkanShaderLib::VulkanShaderLib(const VulkanDevice& device, const ext::filesystem::path& filepath)
    : ShaderLib(filepath), m_device(&device)
{
    if (m_spirvBytes.empty())
        throw ext::runtime_error("No SPIR-V shader found in the package");

    auto shaderModuleCreateInfo = vk::ShaderModuleCreateInfo{}
        .setCodeSize(m_spirvBytes.size())
        .setPCode(reinterpret_cast<const uint32_t*>(m_spirvBytes.data()));

    m_vkShaderModule = m_device->vkDevice().createShaderModule(shaderModuleCreateInfo);
}

VulkanShaderFunction& VulkanShaderLib::getFunction(const ext::string& name)
{
    auto it = m_shaderFunctions.find(name);
    if (it == m_shaderFunctions.end())
    {
        auto [newIt, res] = m_shaderFunctions.emplace(ext::make_pair(name, VulkanShaderFunction(&m_vkShaderModule, name)));
        assert(res);
        it = newIt;
    }
    return it->second;
}

VulkanShaderLib::~VulkanShaderLib()
{
    m_shaderFunctions.clear();
    m_device->vkDevice().destroyShaderModule(m_vkShaderModule);
}

} // namespace gfx
