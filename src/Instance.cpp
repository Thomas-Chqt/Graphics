/*
 * ---------------------------------------------------
 * Instance.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/27 13:49:51
 * ---------------------------------------------------
 */

#include "Graphics/Instance.hpp"

#if defined(GFX_BUILD_METAL)
    #include "Metal/MetalInstance.hpp"
#endif

#if defined(GFX_BUILD_VULKAN)
    #include "Vulkan/VulkanInstance.hpp"
#endif

namespace gfx
{

std::unique_ptr<Instance> Instance::newInstance(const Descriptor& desc)
{
    if (const char* val = std::getenv("GFX_USED_API")) // NOLINT(concurrency-mt-unsafe)
    {
#if defined(GFX_BUILD_METAL)
        if (std::strcmp(val, "METAL") == 0)
        {
            std::println("using metal");
            return newMetalInstance(desc);
        }
#endif
#if defined(GFX_BUILD_VULKAN)
        if (std::strcmp(val, "VULKAN") == 0)
        {
            std::println("using vulkan");
            return newVulkanInstance(desc);
        }
#endif
        throw std::runtime_error(std::format("unknown api name: {}", val));
    }
    else
    {
#if defined(GFX_BUILD_METAL)
        return newMetalInstance(desc);
#endif

#if defined(GFX_BUILD_VULKAN)
        return newVulkanInstance(desc);
#endif
        throw std::runtime_error("unable to define default api");
    }
}

#if defined(GFX_BUILD_METAL)
std::unique_ptr<Instance> Instance::newMetalInstance(const Descriptor& desc)
{
    return std::make_unique<MetalInstance>(desc);
}
#endif

#if defined(GFX_BUILD_VULKAN)
std::unique_ptr<Instance> Instance::newVulkanInstance(const Descriptor& desc)
{
    return std::make_unique<VulkanInstance>(desc);
}
#endif

}
