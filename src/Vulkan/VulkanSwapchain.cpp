/*
 * ---------------------------------------------------
 * VulkanSwapchain.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/01 13:39:49
 * ---------------------------------------------------
 */

#define VULKAN_HPP_NO_CONSTRUCTORS

#include "Graphics/Drawable.hpp"

#include "Vulkan/VulkanSwapchain.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"
#include "Vulkan/VulkanSurface.hpp"
#include "Vulkan/VulkanEnums.hpp"
#include "Vulkan/VulkanDrawable.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <cassert>
    #include <vector>
    #include <algorithm>
    #include <limits>
    #include <cstddef>
    #include <memory>
    #include <cstdint>
    #include <stdexcept>
    #include <utility>
    namespace ext = std;
#endif

namespace gfx
{

VulkanSwapchain::VulkanSwapchain(const VulkanDevice& device, ext::vector<ext::shared_ptr<VulkanDrawable>>&& drawables, const Descriptor& desc)
    : m_device(&device), m_drawables(std::move(drawables))
{
    assert(desc.surface);
    const vk::SurfaceKHR& vkSurface = dynamic_cast<const VulkanSurface&>(*desc.surface).vkSurface();

    const VulkanPhysicalDevice& vkPhysicalDevice = m_device->physicalDevice();
    vk::SurfaceCapabilitiesKHR surfaceCapabilities = vkPhysicalDevice.getSurfaceCapabilitiesKHR(vkSurface);
    // TODO : chech image count

    ext::vector<vk::SurfaceFormatKHR> surfaceFormats = vkPhysicalDevice.getSurfaceFormatsKHR(vkSurface);
    assert(ext::ranges::any_of(surfaceFormats, [&desc](auto& f){
        return f.format == toVkFormat(desc.pixelFormat) && f.colorSpace == toVkColorSpaceKHR(desc.pixelFormat);
    }));

    ext::vector<vk::PresentModeKHR> surfacePresentModes = vkPhysicalDevice.getSurfacePresentModesKHR(vkSurface);
    assert(ext::ranges::any_of(surfacePresentModes, [&desc](auto& m){return m == toVkPresentModeKHR(desc.presentMode);}));

    vk::Extent2D extent;
    if (surfaceCapabilities.currentExtent.width != ext::numeric_limits<uint32_t>::max())
        extent = surfaceCapabilities.currentExtent;
    else
    {
        extent.width = ext::clamp(desc.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
        extent.height = ext::clamp(desc.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
    }

    vk::SwapchainCreateInfoKHR swapchainCreateInfo = {
        .surface = vkSurface,
        .minImageCount = desc.imageCount,
        .imageFormat = toVkFormat(desc.pixelFormat),
        .imageColorSpace = toVkColorSpaceKHR(desc.pixelFormat),
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .preTransform = surfaceCapabilities.currentTransform,
        .presentMode = toVkPresentModeKHR(desc.presentMode),
        .clipped = VK_TRUE};

    m_vkSwapchain = m_device->vkDevice().createSwapchainKHR(swapchainCreateInfo);

    std::vector<vk::Image> vkSwapchainImages = m_device->vkDevice().getSwapchainImagesKHR(m_vkSwapchain);
    m_swapchainImages.resize(vkSwapchainImages.size());

    for (uint32_t i = 0; auto& image : vkSwapchainImages)
    {
        SwapchainImage::Descriptor swapchainImageDescriptor = {
            .textureDescriptor = Texture::Descriptor{
                .width = extent.width, .height = extent.height,
                .pixelFormat = desc.pixelFormat
            },
            .swapchain = this,
            .index = i
        };
        m_swapchainImages[i] = ext::make_shared<SwapchainImage>(*m_device, image, swapchainImageDescriptor);
        i++;
    }
}

ext::shared_ptr<Drawable> VulkanSwapchain::nextDrawable(void)
{
    ext::shared_ptr<VulkanDrawable> drawable = m_drawables[m_nextDrawableIndex];
    m_nextDrawableIndex = (m_nextDrawableIndex + 1) % m_drawables.size();

    uint64_t timeout = ext::numeric_limits<uint64_t>::max();
    auto& semaphore = drawable->imageAvailableSemaphore();

    auto result = m_device->vkDevice().acquireNextImageKHR(m_vkSwapchain, timeout, semaphore, nullptr);

    switch (result.result)
    {
    case vk::Result::eSuccess:
        drawable->setSwapchainImage(m_swapchainImages[result.value]);
        return drawable;
    default:
        throw ext::runtime_error("error");
    }
}

VulkanSwapchain::~VulkanSwapchain()
{
    m_drawables.clear();
    m_swapchainImages.clear();
    m_device->vkDevice().destroySwapchainKHR(m_vkSwapchain);
}

} // namespace gfx
