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
#include "Graphics/PhysicalDevice.hpp"
#include "Graphics/Device.hpp"

#include "Vulkan/VulkanPhysicalDevice.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    #include "UtilsCPP/memory.hpp"
    namespace ext = utl;
#else
    #include <memory>
    namespace ext = std;
#endif

#if defined(GFX_GLFW_ENABLED)
    class GLFWwindow;
#endif

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

    const ext::vector<PhysicalDevice*> listPhysicalDevices() override;

    ext::unique_ptr<Device> newDevice(const Device::Descriptor&, const PhysicalDevice* = nullptr) override;

    ~VulkanInstance();

private:
    VulkanPhysicalDevice* findSuitableDevice(const Device::Descriptor&);

    vk::Instance m_vkInstance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    ext::vector<ext::unique_ptr<VulkanPhysicalDevice>> m_physicalDevices;

public:
    VulkanInstance& operator=(const VulkanInstance&) = delete;
    VulkanInstance& operator=(VulkanInstance&&) = delete;
};

} // namespace gfx

#endif // VULKANINSTANCE_HPP
