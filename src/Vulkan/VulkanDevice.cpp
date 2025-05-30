/*
 * ---------------------------------------------------
 * VulkanDevice.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/28 06:27:45
 * ---------------------------------------------------
 */

#include "Graphics/Device.hpp"

#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"

namespace gfx
{

VulkanDevice::VulkanDevice(const VulkanPhysicalDevice& phyDevice, const Device::Descriptor& desc)
{
}

VulkanDevice::~VulkanDevice()
{
    m_vkDevice.destroy();
}

}
