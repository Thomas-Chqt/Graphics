/*
 * ---------------------------------------------------
 * VulkanSwapchain.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/01 13:39:49
 * ---------------------------------------------------
 */

#include "Graphics/Drawable.hpp"

#include "Vulkan/VulkanSwapchain.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Texture.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"
#include "Vulkan/VulkanSurface.hpp"
#include "Vulkan/VulkanEnums.hpp"
#include "Vulkan/VulkanDrawable.hpp"
#include "common.hpp"

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
    #include <ranges>
    #include <map>
    namespace ext = std;
#endif

namespace gfx
{

VulkanSwapchain::VulkanSwapchain(const VulkanDevice* device, const Descriptor& desc)
    : m_device(device)
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


    auto swapchainCreateInfo = vk::SwapchainCreateInfoKHR{}
        .setSurface(vkSurface)
        .setMinImageCount(desc.imageCount)
        .setImageFormat(toVkFormat(desc.pixelFormat))
        .setImageColorSpace(toVkColorSpaceKHR(desc.pixelFormat))
        .setImageExtent(extent)
        .setImageArrayLayers(1)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
        .setPreTransform(surfaceCapabilities.currentTransform)
        .setPresentMode(toVkPresentModeKHR(desc.presentMode))
        .setClipped(vk::True);
    
    static ext::map<const vk::SurfaceKHR*, vk::SwapchainKHR*> s_oldSwapchains;
    if (s_oldSwapchains.contains(&vkSurface))
        swapchainCreateInfo.setOldSwapchain(*s_oldSwapchains[&vkSurface]);

    auto vkSwapchain = m_device->vkDevice().createSwapchainKHR(swapchainCreateInfo);
    auto vkSwapchainPtr = ext::shared_ptr<vk::SwapchainKHR>(
        new vk::SwapchainKHR(ext::move(vkSwapchain)),
        [device=m_device](vk::SwapchainKHR* ptr){
            device->vkDevice().destroySwapchainKHR(*ptr);
            delete ptr; // NOLINT
        }
    );
    m_vkSwapchain = vkSwapchainPtr.get();
    s_oldSwapchains[&vkSurface] = m_vkSwapchain;

    Texture::Descriptor swapchainImageTexDesc = {
        .width = extent.width, .height = extent.height,
        .pixelFormat = desc.pixelFormat,
        .usages = TextureUsage::colorAttachment,
        .storageMode = ResourceStorageMode::deviceLocal
    };

    m_swapchainImages = m_device->vkDevice().getSwapchainImagesKHR(*vkSwapchainPtr)
        | ext::views::transform([&](vk::Image& vkImage) { return ext::make_shared<SwapchainImage>(m_device, ext::move(vkImage), vkSwapchainPtr, swapchainImageTexDesc); })
        | ext::ranges::to<ext::vector>();

    for (auto& drawable : m_drawables)
        drawable = ext::make_shared<VulkanDrawable>(m_device);
}

ext::shared_ptr<Drawable> VulkanSwapchain::nextDrawable()
{
    ext::shared_ptr<VulkanDrawable> drawable = m_drawables.at(m_nextDrawableIndex);
    m_nextDrawableIndex = (m_nextDrawableIndex + 1) % m_drawables.size();

    uint64_t timeout = ext::numeric_limits<uint64_t>::max();
    auto& semaphore = drawable->imageAvailableSemaphore();

    auto [result, value] = m_device->vkDevice().acquireNextImageKHR(*m_vkSwapchain, timeout, semaphore, nullptr);

    switch (result)
    {
    case vk::Result::eSuccess:
        drawable->setSwapchainImage(m_swapchainImages[value], value);
        return drawable;
    case vk::Result::eSuboptimalKHR:
    case vk::Result::eErrorOutOfDateKHR:
        return nullptr;
    default:
        throw std::runtime_error("Failed to acquire swapchain image!");
    }
}

VulkanSwapchain::~VulkanSwapchain()
{
    m_drawables = PerFrameInFlight<ext::shared_ptr<VulkanDrawable>>();
    m_swapchainImages.clear();
}

} // namespace gfx
