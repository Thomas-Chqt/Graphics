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

#include "Graphics/Texture.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanFramebuffer.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    #include <vector>
    namespace ext = std;
#endif

namespace gfx
{

class VulkanSwapchain : public Swapchain
{
public:
    VulkanSwapchain(const VulkanSwapchain&) = delete;
    VulkanSwapchain(VulkanSwapchain&&) = delete;

    VulkanSwapchain(const VulkanDevice&, const Swapchain::Descriptor&);

    const Framebuffer& nextFrameBuffer(void) override;

    ~VulkanSwapchain();

private:
    const VulkanDevice* m_device;

    vk::SwapchainKHR m_vkSwapchain;
    ext::vector<ext::shared_ptr<Texture>> m_swapchainTextures;
    ext::vector<ext::unique_ptr<VulkanFramebuffer>> m_frameBuffers;

public:
    VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;
    VulkanSwapchain& operator=(VulkanSwapchain&&) = delete;
};

} // namespace gfx

#endif // VULKANSWAPCHAIN_HPP
