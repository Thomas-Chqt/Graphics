/*
 * ---------------------------------------------------
 * Device.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/21 22:16:47
 * ---------------------------------------------------
 */

#include "Graphics/Device.hpp"

#if defined(GFX_USE_UTILSCPP)
    #include "UtilsCPP/memory.hpp"
    namespace ext = utl;
#else
    #include <memory>
    namespace ext = std;
#endif

#if defined(GFX_BUILD_METAL)
    #include "Metal/MetalDevice.hpp"
#endif

#if defined(GFX_BUILD_VULKAN)
    #include "Vulkan/VulkanDevice.hpp"
#endif

namespace gfx
{

ext::unique_ptr<Device> Device::createDevice()
{
#if defined(GFX_BUILD_METAL) && defined(GFX_BUILD_VULKAN)
    //if (const char* val = std::getenv("GFX_USED_API"))
    //{
    //    if (utils::String(val) == utils::String("VULKAN"))
    //        return createVulkanDevice();
    //}
    return createMetalDevice();
#elif defined(GFX_BUILD_METAL)
    return createMetalDevice();
#elif defined(GFX_BUILD_VULKAN)
    return createVulkanDevice()
#endif
}

#if defined(GFX_BUILD_METAL)
ext::unique_ptr<Device> Device::createMetalDevice(void)
{
    return ext::unique_ptr<Device>(new MetalDevice);
}
#endif

#if defined(GFX_BUILD_VULKAN)
ext::unique_ptr<Device> Device::createVulkanDevice(void)
{
    return ext::unique_ptr<Device>(new VulkanDevice);
}
#endif

} // namespace gfx
