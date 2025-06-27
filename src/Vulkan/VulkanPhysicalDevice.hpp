/*
 * ---------------------------------------------------
 * VulkanPhysicalDevice.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/30 14:46:35
 * ---------------------------------------------------
 */

#ifndef VULKANPHYSICALDEVICE_HPP
#define VULKANPHYSICALDEVICE_HPP

#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/QueueFamily.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
#else
    #include <vector>
    namespace ext = std;
#endif

namespace gfx
{

class VulkanPhysicalDevice : public vk::PhysicalDevice
{
public:
    using vk::PhysicalDevice::PhysicalDevice;

    bool isSuitable(const VulkanDevice::Descriptor&) const;

    ext::vector<QueueFamily> getQueueFamilies() const;
    bool suportExtensions(const ext::vector<const char*>& extensionNames) const;
};

} // namespace gfx

#endif // VULKANPHYSICALDEVICE_HPP
