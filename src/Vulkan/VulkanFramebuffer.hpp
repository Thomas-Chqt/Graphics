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

#include "Vulkan/VulkanDevice.hpp"
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

class VulkanFramebuffer : public Framebuffer
{
public:
    VulkanFramebuffer() = delete;
    VulkanFramebuffer(const VulkanFramebuffer&) = delete;
    VulkanFramebuffer(VulkanFramebuffer&&)      = delete;

    VulkanFramebuffer(const VulkanDevice&, const Framebuffer::Descriptor&);

    ~VulkanFramebuffer();

private:
    const VulkanDevice* m_device;
    ext::vector<ext::shared_ptr<VulkanTexture>> m_attachments;
    ext::vector<vk::ImageView> m_imageViews;
    vk::Framebuffer m_vkFramebuffer;
    
public:
    VulkanFramebuffer& operator = (const VulkanFramebuffer&) = delete;
    VulkanFramebuffer& operator = (VulkanFramebuffer&&)      = delete;
};

}

#endif // VULKANFRAMEBUFFER_HPP
