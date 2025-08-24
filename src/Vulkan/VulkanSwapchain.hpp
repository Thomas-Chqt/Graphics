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
#include "Graphics/Drawable.hpp"

#include "common.hpp"
#include "Vulkan/SwapchainImage.hpp"
#include "Vulkan/VulkanDrawable.hpp"

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

    VulkanSwapchain(const VulkanDevice*, const Swapchain::Descriptor&);

    ext::shared_ptr<Drawable> nextDrawable() override;

    ~VulkanSwapchain() override;

private:
    const VulkanDevice* m_device;
    
    vk::SwapchainKHR* m_vkSwapchain;
    ext::vector<ext::shared_ptr<SwapchainImage>> m_swapchainImages;

    PerFrameInFlight<ext::shared_ptr<VulkanDrawable>> m_drawables;
    uint32_t m_nextDrawableIndex = 0;

public:
    VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;
    VulkanSwapchain& operator=(VulkanSwapchain&&) = delete;
};

} // namespace gfx

#endif // VULKANSWAPCHAIN_HPP
