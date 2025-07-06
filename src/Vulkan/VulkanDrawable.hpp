/*
 * ---------------------------------------------------
 * VulkanDrawable.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/21 16:39:22
 * ---------------------------------------------------
 */

#ifndef VULKANDRAWABLE_HPP
#define VULKANDRAWABLE_HPP

#include "Graphics/Drawable.hpp"
#include "Graphics/Texture.hpp"

#include "SwapchainImage.hpp"
#include "Vulkan/VulkanTexture.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    #include <cstdint>
    namespace ext = std;
#endif

namespace gfx
{

class VulkanDevice;

class VulkanDrawable : public Drawable
{
public:
    VulkanDrawable() = delete;
    VulkanDrawable(const VulkanDrawable&) = delete;
    VulkanDrawable(VulkanDrawable&&) = delete;

    VulkanDrawable(const VulkanDevice*);

    inline ext::shared_ptr<Texture> texture(void) const override { return m_swapchainImage; }
    inline ext::shared_ptr<SwapchainImage> swapchainImage(void) const { return m_swapchainImage; }

    void setSwapchainImage(const ext::shared_ptr<SwapchainImage>& swapchainImage, uint32_t imageIndex);

    inline const vk::SwapchainKHR& swapchain(void) const { return m_swapchainImage->swapchain(); }
    inline uint32_t imageIndex(void) const { return m_imageIndex; }

    inline const vk::Semaphore& imageAvailableSemaphore(void) const { return m_imageAvailableSemaphore; }
    inline const vk::Semaphore& imagePresentableSemaphore(void) const { return m_swapchainImage->imagePresentableSemaphore(); }

    ~VulkanDrawable();

private:
    const VulkanDevice* m_device;
    vk::Semaphore m_imageAvailableSemaphore;

    ext::shared_ptr<SwapchainImage> m_swapchainImage;
    uint32_t m_imageIndex;

public:
    VulkanDrawable& operator=(const VulkanDrawable&) = delete;
    VulkanDrawable& operator=(VulkanDrawable&&) = delete;
};

}

#endif // VULKANDRAWABLE_HPP
