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

#if defined(GFX_USE_UTILSCPP)
    #include "UtilsCPP/memory.hpp"
    namespace ext = utl;
#else
    #include <memory>
    namespace ext = std;
#endif

namespace gfx
{

class Device
{
public:
    Device(const Device&) = delete;
    Device(Device&&)      = delete;

    static ext::unique_ptr<Device> createDevice(void);

#if defined (GFX_BUILD_METAL)
    static ext::unique_ptr<Device> createMetalDevice(void);
#endif
#if defined (GFX_BUILD_VULKAN)
    static ext::unique_ptr<Device> createVulkanDevice(void);
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
