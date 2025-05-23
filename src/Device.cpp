/*
 * ---------------------------------------------------
 * Device.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/21 22:16:47
 * ---------------------------------------------------
 */

#include "Graphics/Device.hpp"
#include "UtilsCPP/UniquePtr.hpp"
#include "UtilsCPP/String.hpp"

#if defined (GFX_BUILD_METAL)
    #include "Metal/MetalDevice.hpp"
#endif

#if defined (GFX_BUILD_VULKAN)
    #include "Vulkan/VulkanDevice.hpp"
#endif

namespace gfx
{

utils::UniquePtr<Device> Device::createDevice()
{
    #if defined (GFX_BUILD_METAL) && defined (GFX_BUILD_VULKAN)
        if (const char* val = std::getenv("GFX_USED_API"))
        {
            if (utils::String(val) == utils::String("VULKAN"))
                return createVulkanDevice();
        }
        return createMetalDevice();
    #elif defined (GFX_BUILD_METAL)
        return createMetalDevice();
    #elif defined (GFX_BUILD_VULKAN)
        return createVulkanDevice()
    #endif
}

#if defined (GFX_BUILD_METAL)
utils::UniquePtr<Device> Device::createMetalDevice(void)
{
    return utils::UniquePtr<Device>(new MetalDevice);
}
#endif

#if defined (GFX_BUILD_VULKAN)
utils::UniquePtr<Device> Device::createVulkanDevice(void)
{
    return utils::UniquePtr<Device>(new VulkanDevice);
}
#endif

}
