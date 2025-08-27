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

namespace gfx
{

class VulkanDevice;

class SwapchainImage : public VulkanTexture
{
public:
    SwapchainImage() = delete;
    SwapchainImage(const SwapchainImage&) = delete;
    SwapchainImage(SwapchainImage&&) = delete;

    SwapchainImage(const VulkanDevice*, vk::Image&&, const ext::shared_ptr<vk::SwapchainKHR>&, const Texture::Descriptor&);

    inline const vk::SwapchainKHR& swapchain() const { return *m_swapchain; }

    inline const vk::Semaphore& imagePresentableSemaphore() const { return m_imagePresentableSemaphore; }

    inline const vk::Semaphore& imageAvailableSemaphore() const { return *m_imageAvailableSemaphore; }
    inline void setImageAvailableSemaphoreRef(vk::Semaphore* s) { m_imageAvailableSemaphore = s; }

    ~SwapchainImage() override;

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
