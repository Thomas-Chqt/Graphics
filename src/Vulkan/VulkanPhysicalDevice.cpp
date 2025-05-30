/*
 * ---------------------------------------------------
 * VulkanPhysicalDevice.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/30 14:52:32
 * ---------------------------------------------------
 */

#include "Vulkan/VulkanPhysicalDevice.hpp"

#include <vulkan/vulkan.hpp>

namespace gfx
{

VulkanPhysicalDevice::VulkanPhysicalDevice(vk::PhysicalDevice phyDevice)
    : m_vkPhyDevice(phyDevice)
{
}

ext::string VulkanPhysicalDevice::name() const
{
    return "";
}

bool VulkanPhysicalDevice::isSuitable(const Device::Descriptor&) const
{
    return true; // TODO : real check
}

}
