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
#include <vulkan/vulkan.hpp>

#if defined(GFX_GLFW_ENABLED)
    class GLFWwindow;
#endif

namespace gfx
{

class VulkanSurface : public Surface
{
public:
    VulkanSurface() = delete;
    VulkanSurface(const VulkanSurface&) = delete;
    VulkanSurface(VulkanSurface&&) = delete;

#if defined(GFX_GLFW_ENABLED)
    VulkanSurface(vk::Instance&, GLFWwindow*);
#endif

    ~VulkanSurface();

private:
    vk::Instance& m_vkInstance;
    vk::SurfaceKHR m_vkSurface;

public:
    VulkanSurface& operator=(const VulkanSurface&) = delete;
    VulkanSurface& operator=(VulkanSurface&&) = delete;
};

} // namespace gfx

#endif // VULKANSURFACE_HPP
