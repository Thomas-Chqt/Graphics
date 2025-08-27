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

ext::unique_ptr<Instance> Instance::newInstance(const Descriptor& desc)
{
    if (const char* val = ext::getenv("GFX_USED_API")) // NOLINT(concurrency-mt-unsafe)
    {
#if defined(GFX_BUILD_METAL)
        if (ext::strcmp(val, "VULKAN") == 0)
        {
            ext::println("using vulkan");
            return newVulkanInstance(desc);
        }
#endif
#if defined(GFX_BUILD_VULKAN)
        if (ext::strcmp(val, "METAL") == 0)
        {
            ext::println("using metal");
            return newMetalInstance(desc);
        }
#endif
        throw ext::runtime_error(ext::format("unknown api name: {}", val));
    }
    else
    {
#if defined(GFX_BUILD_METAL)
        return newMetalInstance(desc);
#endif

#if defined(GFX_BUILD_VULKAN)
        return newVulkanInstance(desc);
#endif
        throw ext::runtime_error("unable to define default api");
    }
}

#if defined(GFX_BUILD_METAL)
ext::unique_ptr<Instance> Instance::newMetalInstance(const Descriptor& desc)
{
    return ext::make_unique<MetalInstance>(desc);
}
#endif

#if defined(GFX_BUILD_VULKAN)
ext::unique_ptr<Instance> Instance::newVulkanInstance(const Descriptor& desc)
{
    return ext::make_unique<VulkanInstance>(desc);
}
#endif

}
