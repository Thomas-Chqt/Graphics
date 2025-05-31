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

#include "Graphics/PhysicalDevice.hpp"
#include "Graphics/Device.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <string>
    #include <cstdint>
    #include <vector>
    namespace ext = std;
#endif

namespace gfx
{

class VulkanPhysicalDevice;

struct QueueFamily
{
    QueueCapabilityFlag flags;
    uint32_t count;
    uint32_t index;

    bool isCapableOf(const QueueCapability&, const VulkanPhysicalDevice&) const;

    bool operator<(const QueueFamily& other) const { return index < other.index; }
};

class VulkanPhysicalDevice : public PhysicalDevice
{
public:
    VulkanPhysicalDevice(const VulkanPhysicalDevice&) = delete;
    VulkanPhysicalDevice(VulkanPhysicalDevice&&) = delete;

    VulkanPhysicalDevice(vk::PhysicalDevice);

    ext::string name() const override;

    bool isSuitable(const Device::Descriptor&) const override;

    const vk::PhysicalDevice& vkDevice() const { return m_vkPhyDevice; }
    ext::vector<QueueFamily> getQueueFamilies() const;

    ~VulkanPhysicalDevice() = default;

private:
    vk::PhysicalDevice m_vkPhyDevice;

public:
    VulkanPhysicalDevice& operator=(const VulkanPhysicalDevice&) = delete;
    VulkanPhysicalDevice& operator=(VulkanPhysicalDevice&&) = delete;
};

} // namespace gfx

#endif // VULKANPHYSICALDEVICE_HPP
