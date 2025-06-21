/*
 * ---------------------------------------------------
 * VulkanFramebuffer.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/06 06:43:14
 * ---------------------------------------------------
 */

#ifndef VULKANFRAMEBUFFER_HPP
#define VULKANFRAMEBUFFER_HPP

#include "Graphics/Framebuffer.hpp"

#include "Vulkan/VulkanTexture.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    #include <memory>
    namespace ext = std;
#endif

namespace gfx
{

class VulkanDevice;

class VulkanFramebuffer : public Framebuffer
{
public:
    VulkanFramebuffer() = delete;
    VulkanFramebuffer(const VulkanFramebuffer&) = delete;
    VulkanFramebuffer(VulkanFramebuffer&&)      = delete;

    VulkanFramebuffer(const VulkanDevice&, const Framebuffer::Descriptor&);

    ext::vector<ext::shared_ptr<Texture>> colorAttachments(void) override;
    const ext::vector<ext::shared_ptr<Texture>> colorAttachments(void) const override;

    ext::vector<ext::shared_ptr<VulkanTexture>> vkColorAttachments(void) { return m_colorAttachments; }
    const ext::vector<ext::shared_ptr<VulkanTexture>> vkColorAttachments(void) const { return m_colorAttachments; } 


    inline ext::shared_ptr<Texture> colorAttachment(size_t i) override { return m_colorAttachments[i]; }
    inline const ext::shared_ptr<Texture> colorAttachment(size_t i) const override { return m_colorAttachments[i]; }

    inline ext::shared_ptr<VulkanTexture> vkColorAttachment(size_t i) { return m_colorAttachments[i]; }
    inline const ext::shared_ptr<VulkanTexture> vkColorAttachment(size_t i) const { return m_colorAttachments[i]; }


    inline ext::shared_ptr<Texture> depthAttachment(void) override { return m_depthAttachment; }
    inline const ext::shared_ptr<Texture> depthAttachment(void) const override { return m_depthAttachment; };

    inline ext::shared_ptr<VulkanTexture> vkDepthAttachment(void) { return m_depthAttachment; }
    inline const ext::shared_ptr<VulkanTexture> vkDepthAttachment(void) const { return m_depthAttachment; };


    inline const vk::Framebuffer& vkFramebuffer(void) const { return m_vkFramebuffer; };

    ~VulkanFramebuffer();

private:
    const VulkanDevice* m_device;
    ext::vector<ext::shared_ptr<VulkanTexture>> m_colorAttachments;
    ext::shared_ptr<VulkanTexture> m_depthAttachment;
    ext::vector<vk::ImageView> m_imageViews;
    vk::Framebuffer m_vkFramebuffer;
    
public:
    VulkanFramebuffer& operator = (const VulkanFramebuffer&) = delete;
    VulkanFramebuffer& operator = (VulkanFramebuffer&&)      = delete;
};

}

#endif // VULKANFRAMEBUFFER_HPP
