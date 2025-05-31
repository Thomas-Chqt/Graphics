/*
 * ---------------------------------------------------
 * VulkanDevice.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/23 07:29:33
 * ---------------------------------------------------
 */

#ifndef VULKANDEVICE_HPP
#define VULKANDEVICE_HPP

#include "Graphics/Device.hpp"

#include "Vulkan/VulkanPhysicalDevice.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    #include <map>
    namespace ext = std;
#endif

using VulkanQueue = void*;

namespace gfx
{

class VulkanDevice : public Device
{
public:
    VulkanDevice() = delete;
    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice(VulkanDevice&&) = delete;

    VulkanDevice(const VulkanPhysicalDevice&, const Device::Descriptor&);

    ~VulkanDevice();

private:
    vk::Device m_vkDevice;
    ext::map<QueueFamily, ext::vector<vk::Queue>> m_queues;

public:
    VulkanDevice& operator=(const VulkanDevice&) = delete;
    VulkanDevice& operator=(VulkanDevice&&) = delete;
};

} // namespace gfx

#endif // VULKANDEVICE_HPP
