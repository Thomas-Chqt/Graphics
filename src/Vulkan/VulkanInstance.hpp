/*
 * ---------------------------------------------------
 * VulkanInstance.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/27 15:04:36
 * ---------------------------------------------------
 */

#ifndef VULKANINSTANCE_HPP
#define VULKANINSTANCE_HPP

#include "Graphics/Instance.hpp"
#include "Graphics/Surface.hpp"
#include "Graphics/Device.hpp"

#include "Vulkan/VulkanPhysicalDevice.hpp"

namespace gfx
{

class VulkanInstance : public Instance
{
public:
    VulkanInstance() = delete;
    VulkanInstance(const VulkanInstance&) = delete;
    VulkanInstance(VulkanInstance&&) = delete;

    VulkanInstance(const Instance::Descriptor&);

#if defined(GFX_GLFW_ENABLED)
    ext::unique_ptr<Surface> createSurface(GLFWwindow*) override;
#endif

    ext::unique_ptr<Device> newDevice(const Device::Descriptor&) override;

    const vk::Instance& vkInstance() const { return m_vkInstance; }

    ~VulkanInstance() override;

private:
    vk::Instance m_vkInstance;
#if !defined (NDEBUG)
    VkDebugUtilsMessengerEXT m_debugMessenger;
#endif
    ext::vector<VulkanPhysicalDevice> m_physicalDevices;

public:
    VulkanInstance& operator=(const VulkanInstance&) = delete;
    VulkanInstance& operator=(VulkanInstance&&) = delete;
};

} // namespace gfx

#endif // VULKANINSTANCE_HPP
