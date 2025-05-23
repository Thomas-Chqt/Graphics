/*
 * ---------------------------------------------------
 * Device.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/21 11:38:59
 * ---------------------------------------------------
 */

#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <UtilsCPP/UniquePtr.hpp>

namespace gfx
{

class Device
{
public:
    Device(const Device&) = delete;
    Device(Device&&)      = delete;

    static utils::UniquePtr<Device> createDevice(void);

#if defined (GFX_BUILD_METAL)
    static utils::UniquePtr<Device> createMetalDevice(void);
#endif
#if defined (GFX_BUILD_VULKAN)
    static utils::UniquePtr<Device> createVulkanDevice(void);
#endif
    
    virtual ~Device() = default;

protected:
    Device() = default;
    
public:
    Device& operator = (const Device&) = delete;
    Device& operator = (Device&&)      = delete;
};

}

#endif // DEVICE_HPP
