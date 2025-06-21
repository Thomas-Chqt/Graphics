/*
 * ---------------------------------------------------
 * VulkanSwapchain.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/01 13:37:15
 * ---------------------------------------------------
 */

#ifndef VULKANSWAPCHAIN_HPP
#define VULKANSWAPCHAIN_HPP

#include "Graphics/Swapchain.hpp"

#include "Vulkan/VulkanFramebuffer.hpp"
#include "Vulkan/VulkanTexture.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    #include <vector>
    #include <cstdint>
    namespace ext = std;
#endif

namespace gfx
{

class VulkanDevice;

class VulkanSwapchain : public Swapchain
{
public:
    VulkanSwapchain(const VulkanSwapchain&) = delete;
    VulkanSwapchain(VulkanSwapchain&&) = delete;

    VulkanSwapchain(const VulkanDevice&, const Swapchain::Descriptor&);

    ext::shared_ptr<Framebuffer> nextFrameBuffer(void) override;

    const vk::SwapchainKHR& vkSwapchain(void) const { return m_vkSwapchain; }

    inline const uint32_t& currentImageIndex(void) const { return m_imageIndex; }
    inline const VulkanTexture& currentTexture(void) const { return *m_swapchainTextures[m_imageIndex];; }

    ~VulkanSwapchain();

private:
    const VulkanDevice* m_device;

    vk::SwapchainKHR m_vkSwapchain;
    ext::vector<ext::shared_ptr<VulkanTexture>> m_swapchainTextures;
    ext::vector<ext::shared_ptr<VulkanFramebuffer>> m_frameBuffers;

    uint32_t m_imageIndex;

public:
    VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;
    VulkanSwapchain& operator=(VulkanSwapchain&&) = delete;
};

} // namespace gfx

#endif // VULKANSWAPCHAIN_HPP
