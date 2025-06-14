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
#include "Graphics/Framebuffer.hpp"

#include "Vulkan/VulkanSwapchain.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"
#include "Vulkan/VulkanSurface.hpp"
#include "Vulkan/VulkanEnums.hpp"
#include "Vulkan/VulkanFramebuffer.hpp"
#include "Vulkan/VulkanTexture.hpp"

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

    const VulkanPhysicalDevice& vkPhysicalDevice = m_device->physicalDevice();
    vk::SurfaceCapabilitiesKHR surfaceCapabilities = vkPhysicalDevice.getSurfaceCapabilitiesKHR(vkSurface);
    // TODO : chech image count

    ext::vector<vk::SurfaceFormatKHR> surfaceFormats = vkPhysicalDevice.getSurfaceFormatsKHR(vkSurface);
    assert(ext::ranges::any_of(surfaceFormats, [&desc](auto& f){return f.format == toVkFormat(desc.pixelFormat) && f.colorSpace == toVkColorSpaceKHR(desc.pixelFormat);}));

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

    std::vector<vk::Image> swapChainImages = m_device->vkDevice().getSwapchainImagesKHR(m_vkSwapchain);
    m_swapchainTextures.resize(swapChainImages.size());
    m_frameBuffers.resize(swapChainImages.size());

    for (size_t i = 0; auto& image : swapChainImages)
    {
        m_swapchainTextures[i] = ext::make_shared<VulkanTexture>(image, Texture::Descriptor{.pixelFormat = desc.pixelFormat});
        Framebuffer::Descriptor framebufferDescriptor = {
            .width = extent.width, .height = extent.height,
            .renderPass = desc.renderPass,
            .colorAttachments = { m_swapchainTextures[i] }
        };
        m_frameBuffers[i++] = ext::make_unique<VulkanFramebuffer>(*m_device, framebufferDescriptor);
    }
}

const Framebuffer& VulkanSwapchain::nextFrameBuffer(void)
{
    vk::ResultValue<uint32_t> result = m_device->vkDevice().acquireNextImageKHR(m_vkSwapchain, ext::numeric_limits<uint64_t>::max(), nullptr, nullptr);
    switch (result.result)
    {
    case vk::Result::eSuccess:
        return *m_frameBuffers[result.value];
    default:
        throw ext::runtime_error("error");
    }
}

VulkanSwapchain::~VulkanSwapchain()
{
    m_frameBuffers.clear();
    m_swapchainTextures.clear();
    m_device->vkDevice().destroySwapchainKHR(m_vkSwapchain);
}

} // namespace gfx
