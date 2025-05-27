/*
 * ---------------------------------------------------
 * VulkanInstance.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/28 06:24:05
 * ---------------------------------------------------
 */

#include "Vulkan/VulkanInstance.hpp"
#include "Vulkan/VulkanDevice.hpp"

#include "Graphics/Device.hpp"
#include "Graphics/Instance.hpp"

#if defined(GFX_USE_UTILSCPP)
    #include "UtilsCPP/memory.hpp"
namespace ext = utl;
#else
    #include <memory>
namespace ext = std;
#endif

namespace gfx
{

VulkanInstance::VulkanInstance(const Instance::Descriptor&)
{
}

ext::unique_ptr<Device> VulkanInstance::newDevice(const Device::Descriptor& desc)
{
    return ext::make_unique<VulkanDevice>(desc);
}

}
