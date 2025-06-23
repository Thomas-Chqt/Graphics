/*
 * ---------------------------------------------------
 * SwapchainImage.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/21 16:45:19
 * ---------------------------------------------------
 */

#ifndef SWAPCHAINIMAGE_HPP
#define SWAPCHAINIMAGE_HPP

#include "Graphics/Texture.hpp"

#include "Vulkan/VulkanTexture.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
#else
    #include <cstdint>
#endif

namespace gfx
{

class VulkanDevice;
class VulkanSwapchain;

class SwapchainImage : public VulkanTexture
{
public:
    struct Descriptor
    {
        Texture::Descriptor textureDescriptor;
        VulkanSwapchain* swapchain;
        uint32_t index;
    };

public:
    SwapchainImage(const VulkanDevice&, vk::Image, const Descriptor&);

    inline void setImageAvailableSemaphoreRef(vk::Semaphore* s) { m_imageAvailableSemaphore = s; }

    inline const vk::Semaphore& imageAvailableSemaphore(void) const { return *m_imageAvailableSemaphore; }
    inline const vk::Semaphore& imagePresentableSemaphore(void) const { return m_imagePresentableSemaphore; }

    inline const VulkanSwapchain& swapchain(void) const { return *m_swapchain; }
    inline uint32_t imageIndex(void) const { return m_index; }

    ~SwapchainImage();

private:
    VulkanSwapchain* m_swapchain;
    uint32_t m_index;
    vk::Semaphore* m_imageAvailableSemaphore = nullptr;
    vk::Semaphore m_imagePresentableSemaphore;

public:
    SwapchainImage& operator=(const SwapchainImage&) = delete;
    SwapchainImage& operator=(SwapchainImage&&) = delete;
};

} // namespace gfx

#endif // SWAPCHAINIMAGE_HPP
