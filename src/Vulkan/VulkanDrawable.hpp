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

#include "Vulkan/SwapchainImage.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    namespace ext = std;
#endif

namespace gfx
{

class VulkanDevice;

class VulkanDrawable : public Drawable
{
public:
    VulkanDrawable(const VulkanDrawable&) = delete;
    VulkanDrawable(VulkanDrawable&&) = delete;

    VulkanDrawable(const VulkanDevice&);

    ~VulkanDrawable();

    inline ext::shared_ptr<Texture> texture(void) const override { return m_swapchainImage; }

    inline const ext::shared_ptr<SwapchainImage>& swapchainImage(void) const { return m_swapchainImage; }
    
    const vk::Semaphore& imageAvailableSemaphore(void) const { return m_imageAvailableSemaphore; }
    void setSwapchainImage(const ext::shared_ptr<SwapchainImage>&);

private:
    const VulkanDevice* m_device;

    ext::shared_ptr<SwapchainImage> m_swapchainImage;
    vk::Semaphore m_imageAvailableSemaphore;

public:
    VulkanDrawable& operator=(const VulkanDrawable&) = delete;
    VulkanDrawable& operator=(VulkanDrawable&&) = delete;
};

}

#endif // VULKANDRAWABLE_HPP
