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

namespace gfx
{

class VulkanDevice : public Device
{
public:
    VulkanDevice() = delete;
    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice(VulkanDevice&&) = delete;

    VulkanDevice(const Device::Descriptor&);

    ~VulkanDevice() = default;

private:

public:
    VulkanDevice& operator=(const VulkanDevice&) = delete;
    VulkanDevice& operator=(VulkanDevice&&) = delete;
};

} // namespace gfx

#endif // VULKANDEVICE_HPP
