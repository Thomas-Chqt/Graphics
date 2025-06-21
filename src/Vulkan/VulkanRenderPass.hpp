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

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    #include <optional>
    namespace ext = std;
#endif

namespace gfx
{

class VulkanDevice;

class VulkanRenderPass : public RenderPass
{
public:
    VulkanRenderPass(const VulkanRenderPass&) = delete;
    VulkanRenderPass(VulkanRenderPass&&)      = delete;

    VulkanRenderPass(const VulkanDevice&, const RenderPass::Descriptor&);

    const vk::RenderPass& vkRenderPass(void) const { return m_vkRenderPass; }

    inline const ext::vector<AttachmentDescriptor>& colorAttachmentsDesc(void) const { return m_colorAttachmentsDesc; }
    inline const ext::optional<AttachmentDescriptor>& depthAttachmentDesc() const { return m_depthAttachmentDesc; }
    
    ~VulkanRenderPass();

private:
    const VulkanDevice* m_device;
    vk::RenderPass m_vkRenderPass;

    ext::vector<AttachmentDescriptor> m_colorAttachmentsDesc;
    ext::optional<AttachmentDescriptor> m_depthAttachmentDesc;
    
public:
    VulkanRenderPass& operator = (const VulkanRenderPass&) = delete;
    VulkanRenderPass& operator = (VulkanRenderPass&&)      = delete;
};

}

#endif // VULKANRENDERPASS_HPP
