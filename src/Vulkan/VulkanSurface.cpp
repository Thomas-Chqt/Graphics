/*
 * ---------------------------------------------------
 * VulkanSurface.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/30 07:02:58
 * ---------------------------------------------------
 */

#include "Graphics/Device.hpp"

#include "Vulkan/VulkanSurface.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Surface.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"
#include "Vulkan/VulkanEnums.hpp"
#include <optional>
#include <ranges>

namespace gfx
{

VulkanSurface::VulkanSurface(const vk::Instance& instance, VkSurfaceKHR surface)
    : m_vkInstance(&instance), m_vkSurface(surface)
{
}

std::set<SurfaceFormat> VulkanSurface::supportedSurfaceFormat(const Device& aDevice) const
{
    const auto& device = dynamic_cast<const VulkanDevice&>(aDevice);

    std::set<SurfaceFormat> surfaceFormats;
    for (const vk::SurfaceFormatKHR& format : device.physicalDevice().getSurfaceFormatsKHR(m_vkSurface))
    {
        SurfaceFormat surfaceFormat{};
        if (std::optional<PixelFormat> pixelFormat = toOptPixelFormat(format.format))
            surfaceFormat.pixelFormat = *pixelFormat;
        else
            continue;
        if (std::optional<ColorSpace> colorSpace = toOptColorSpace(format.colorSpace))
            surfaceFormat.colorSpace = *colorSpace;
        else
            continue;
        surfaceFormats.insert(surfaceFormat);
    }
    return surfaceFormats;
}

std::set<PresentMode> VulkanSurface::supportedPresentModes(const Device& _device) const
{
    const auto& device = dynamic_cast<const VulkanDevice&>(_device);

    std::set<PresentMode> modes;
    for (const vk::PresentModeKHR& mode : device.physicalDevice().getSurfacePresentModesKHR(m_vkSurface))
    {
        switch (mode)
        {
        case vk::PresentModeKHR::eFifo:
        case vk::PresentModeKHR::eMailbox:
            modes.insert(toPresentMode(mode));
        default:
            break;
        }
    }
    return modes;
}

VulkanSurface::~VulkanSurface()
{
    assert(m_vkInstance);
    m_vkInstance->destroySurfaceKHR(m_vkSurface);
}

}
