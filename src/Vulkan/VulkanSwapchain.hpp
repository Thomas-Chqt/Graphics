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

#include "Vulkan/SwapchainImage.hpp"
#include "Vulkan/VulkanDrawable.hpp"
#include <vector>

namespace gfx
{

class VulkanDevice;


class VulkanSwapchain : public Swapchain
{
public:
    VulkanSwapchain(const VulkanSwapchain&) = delete;
    VulkanSwapchain(VulkanSwapchain&&) = delete;

    VulkanSwapchain(const VulkanDevice*, const Swapchain::Descriptor&);

    std::shared_ptr<Drawable> nextDrawable() override;

    ~VulkanSwapchain() override = default;

private:
    const VulkanDevice* m_device;

    vk::SwapchainKHR* m_vkSwapchain;
    std::vector<std::shared_ptr<SwapchainImage>> m_swapchainImages;
    std::vector<std::shared_ptr<VulkanDrawable>> m_drawables;
    uint32_t m_nextDrawableIndex = 0;

public:
    VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;
    VulkanSwapchain& operator=(VulkanSwapchain&&) = delete;
};

} // namespace gfx

#endif // VULKANSWAPCHAIN_HPP
