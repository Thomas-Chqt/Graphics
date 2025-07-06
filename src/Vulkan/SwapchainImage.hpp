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
    #include <memory>
    namespace ext = std;
#endif

namespace gfx
{

class VulkanDevice;

class SwapchainImage : public VulkanTexture
{
public:
    SwapchainImage(const VulkanDevice*, vk::Image&&, const ext::shared_ptr<vk::SwapchainKHR>&, const Texture::Descriptor&);

    inline const vk::SwapchainKHR& swapchain(void) const { return *m_swapchain; }

    inline void setImageAvailableSemaphoreRef(vk::Semaphore* s) { m_imageAvailableSemaphore = s; }

    inline const vk::Semaphore& imageAvailableSemaphore(void) const { return *m_imageAvailableSemaphore; }
    inline const vk::Semaphore& imagePresentableSemaphore(void) const { return m_imagePresentableSemaphore; }

    ~SwapchainImage();

private:
    ext::shared_ptr<vk::SwapchainKHR> m_swapchain;
    vk::Semaphore m_imagePresentableSemaphore;
    vk::Semaphore* m_imageAvailableSemaphore = nullptr;

public:
    SwapchainImage& operator=(const SwapchainImage&) = delete;
    SwapchainImage& operator=(SwapchainImage&&) = delete;
};

} // namespace gfx

#endif // SWAPCHAINIMAGE_HPP
