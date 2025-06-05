/*
 * ---------------------------------------------------
 * VulkanRenderPass.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/05 06:31:42
 * ---------------------------------------------------
 */

#ifndef VULKANRENDERPASS_HPP
#define VULKANRENDERPASS_HPP

#include "Graphics/RenderPass.hpp"

#include "Vulkan/VulkanDevice.hpp"

#include <vulkan/vulkan.hpp>

namespace gfx
{

class VulkanRenderPass : public RenderPass
{
public:
    VulkanRenderPass(const VulkanRenderPass&) = delete;
    VulkanRenderPass(VulkanRenderPass&&)      = delete;

    VulkanRenderPass(const VulkanDevice&, const RenderPass::Descriptor&);
    
    ~VulkanRenderPass();

private:
    const VulkanDevice* m_device;
    vk::RenderPass m_vkRenderPass;
    
public:
    VulkanRenderPass& operator = (const VulkanRenderPass&) = delete;
    VulkanRenderPass& operator = (VulkanRenderPass&&)      = delete;
};

}

#endif // VULKANRENDERPASS_HPP
