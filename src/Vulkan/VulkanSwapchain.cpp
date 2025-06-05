/*
 * ---------------------------------------------------
 * VulkanSwapchain.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/01 13:39:49
 * ---------------------------------------------------
 */

#define VULKAN_HPP_NO_CONSTRUCTORS

#include "Graphics/Swapchain.hpp"

#include "Vulkan/VulkanSwapchain.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanSurface.hpp"
#include "Vulkan/VulkanEnums.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <cassert>
    #include <vector>
    #include <algorithm>
    #include <limits>
    #include <cstddef>
    #include <array>
    namespace ext = std;
#endif

namespace gfx
{

VulkanSwapchain::VulkanSwapchain(const VulkanDevice& device, const Swapchain::Descriptor& desc)
    : m_device(&device)
{
    assert(desc.surface);
    const vk::SurfaceKHR& vkSurface = dynamic_cast<const VulkanSurface&>(*desc.surface).vkSurface();

    assert(desc.renderPass);

    const vk::PhysicalDevice& vkPhysicalDevice = m_device->physicalDevice().vkDevice();
    vk::SurfaceCapabilitiesKHR surfaceCapabilities = vkPhysicalDevice.getSurfaceCapabilitiesKHR(vkSurface);
    // TODO : chech image count

    ext::vector<vk::SurfaceFormatKHR> surfaceFormats = vkPhysicalDevice.getSurfaceFormatsKHR(vkSurface);
    assert(ext::ranges::any_of(surfaceFormats, [&desc](auto& f){return f.format == toVkFormat(desc.pixelFormat) && f.colorSpace == toVkColorSpaceKHR(desc.pixelFormat);}));

    ext::vector<vk::PresentModeKHR> surfacePresentModes = vkPhysicalDevice.getSurfacePresentModesKHR(vkSurface);
    assert(ext::ranges::any_of(surfacePresentModes, [&desc](auto& m){return m == toVkPresentModeKHR(desc.presentMode);}));

    createSwapchain(desc);
    createImageViews(desc);
    createFramebuffers(desc);
}

VulkanSwapchain::~VulkanSwapchain()
{
    for (auto& imageView : m_imageViews)
        m_device->vkDevice().destroyImageView(imageView);
    m_device->vkDevice().destroySwapchainKHR(m_vkSwapchain);
}

void VulkanSwapchain::createSwapchain(const Swapchain::Descriptor& desc)
{
    const vk::SurfaceKHR& vkSurface = dynamic_cast<const VulkanSurface&>(*desc.surface).vkSurface();
    vk::SurfaceCapabilitiesKHR surfaceCapabilities = m_device->physicalDevice().vkDevice().getSurfaceCapabilitiesKHR(vkSurface);

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
    m_swapchainImageFormat = vk::SurfaceFormatKHR(toVkFormat(desc.pixelFormat), toVkColorSpaceKHR(desc.pixelFormat));
    m_swapchainExtent = extent;
}

void VulkanSwapchain::createImageViews(const Swapchain::Descriptor& desc)
{
    std::vector<vk::Image> swapChainImages = m_device->vkDevice().getSwapchainImagesKHR(m_vkSwapchain);
    m_imageViews.resize(swapChainImages.size());

    for (size_t i = 0; auto& image : swapChainImages)
    {
        vk::ImageViewCreateInfo imageViewCreateInfo = {
            .image = image,
            .viewType = vk::ImageViewType::e2D,
            .format = m_swapchainImageFormat.format,
            .subresourceRange = vk::ImageSubresourceRange{
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1}};
        m_imageViews[i++] = m_device->vkDevice().createImageView(imageViewCreateInfo);
    }
}

void VulkanSwapchain::createFramebuffers(const Swapchain::Descriptor& desc)
{
}

} // namespace gfx
