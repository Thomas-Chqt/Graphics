/*
 * ---------------------------------------------------
 * VulkanSurface.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/30 06:56:38
 * ---------------------------------------------------
 */

#ifndef VULKANSURFACE_HPP
#define VULKANSURFACE_HPP

#include "Graphics/Surface.hpp"

namespace gfx
{

class Device;

class VulkanSurface : public Surface
{
public:
    VulkanSurface() = delete;
    VulkanSurface(const VulkanSurface&) = delete;
    VulkanSurface(VulkanSurface&&) = delete;

    VulkanSurface(const vk::Instance&, VkSurfaceKHR);

    std::set<SurfaceFormat> supportedSurfaceFormat(const Device&) const override;
    std::set<PresentMode> supportedPresentModes(const Device&) const override;

    const vk::SurfaceKHR& vkSurface() const { return m_vkSurface; }

    ~VulkanSurface() override;

private:
    const vk::Instance* m_vkInstance;
    vk::SurfaceKHR m_vkSurface;

public:
    VulkanSurface& operator=(const VulkanSurface&) = delete;
    VulkanSurface& operator=(VulkanSurface&&) = delete;
};

} // namespace gfx

#endif // VULKANSURFACE_HPP
