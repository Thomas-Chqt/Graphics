/*
 * ---------------------------------------------------
 * VulkanShaderLib.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/26 07:45:10
 * ---------------------------------------------------
 */

#ifndef VULKANSHADERLIB_HPP
#define VULKANSHADERLIB_HPP

#include "Graphics/ShaderLib.hpp"

#include "Vulkan/VulkanShaderFunction.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <filesystem>
    #include <string>
    #include <map>
    namespace ext = std;
#endif

namespace gfx
{

class VulkanDevice;

class VulkanShaderLib : public ShaderLib
{
public:
    VulkanShaderLib() = delete;
    VulkanShaderLib(const VulkanShaderLib&) = delete;
    VulkanShaderLib(VulkanShaderLib&&) = delete;

    VulkanShaderLib(const VulkanDevice&, const ext::filesystem::path&);

    VulkanShaderFunction& getFunction(const ext::string&) override;

    ~VulkanShaderLib();

private:
    const VulkanDevice* m_device;
    vk::ShaderModule m_vkShaderModule;

    ext::map<ext::string, VulkanShaderFunction> m_shaderFunctions;

public:
    VulkanShaderLib& operator=(const VulkanShaderLib&) = delete;
    VulkanShaderLib& operator=(VulkanShaderLib&&) = delete;
};

} // namespace gfx

#endif // VULKANSHADERLIB_HPP
