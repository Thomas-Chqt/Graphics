/*
 * ---------------------------------------------------
 * Instance.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/27 13:49:51
 * ---------------------------------------------------
 */

#include "Graphics/Instance.hpp"

#if defined(GFX_USE_UTILSCPP)
    #include "UtilsCPP/memory.hpp"
    namespace ext = utl;
#else
    #include <string>
    #include <memory>
    #include <cstdlib>
    #include <iostream>
    namespace ext = std;
#endif

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
#if defined(GFX_BUILD_METAL) && defined(GFX_BUILD_VULKAN)
    if (const char* val = ext::getenv("GFX_USED_API"))
    {
        if (ext::string(val) == ext::string("VULKAN"))
        {
            ext::cout << "using vulkan" << ext::endl;
            return newVulkanInstance(desc);
        }
        ext::cout << "using metal" << ext::endl;
    }
    return newMetalInstance(desc);
#elif defined(GFX_BUILD_METAL)
    return newMetalInstance(desc);
#elif defined(GFX_BUILD_VULKAN)
    return newVulkanInstance(desc)
#endif
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
