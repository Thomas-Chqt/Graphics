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

#include "Vulkan/SwapchainImage.hpp"
#include "Vulkan/VulkanTexture.hpp"

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

    inline ext::shared_ptr<Texture> texture() const override { return m_swapchainImage; }
    inline ext::shared_ptr<VulkanTexture> vulkanTexture() const { return m_swapchainImage; }
    inline ext::shared_ptr<SwapchainImage> swapchainImage() const { return m_swapchainImage; }

    void setSwapchainImage(const ext::shared_ptr<SwapchainImage>& swapchainImage, uint32_t imageIndex);

    inline const vk::SwapchainKHR& swapchain() const { return m_swapchainImage->swapchain(); }
    inline uint32_t imageIndex() const { return m_imageIndex; }

    inline const vk::Semaphore& imageAvailableSemaphore() { return m_imageAvailableSemaphore; }
    inline const vk::Semaphore& imagePresentableSemaphore() const { return m_swapchainImage->imagePresentableSemaphore(); }

    ~VulkanDrawable() override;

private:
    const VulkanDevice* m_device;
    vk::Semaphore m_imageAvailableSemaphore;

    ext::shared_ptr<SwapchainImage> m_swapchainImage;
    uint32_t m_imageIndex = 0;

public:
    VulkanDrawable& operator=(const VulkanDrawable&) = delete;
    VulkanDrawable& operator=(VulkanDrawable&&) = delete;
};

}

#endif // VULKANDRAWABLE_HPP
