/*
 * ---------------------------------------------------
 * MetalInstance.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/28 06:18:35
 * ---------------------------------------------------
 */

#include "Graphics/PhysicalDevice.hpp"
#include "Graphics/Device.hpp"

#include "Metal/MetalInstance.hpp"
#include "Metal/MetalSurface.hpp"
#include "Metal/MetalPhysicalDevice.hpp"
#include "Metal/MetalDevice.hpp"

#import <Metal/Metal.h>
#import <TargetConditionals.h>

#if defined(GFX_USE_UTILSCPP)
    #include "UtilsCPP/memory.hpp"
    namespace ext = utl;
#else
    #include <memory>
    #include <vector>
    #include <string>
    #include <cassert>
    #include <stdexcept>
    namespace ext = std;
#endif

#if defined(GFX_GLFW_ENABLED)
    class GLFWwindow;
#endif

namespace gfx
{

MetalInstance::MetalInstance(const Instance::Descriptor& desc)
    : m_appName(desc.appName),
      m_appVersion{desc.appVersion[0], desc.appVersion[1], desc.appVersion[2]},
      m_engineName(desc.engineName),
      m_engineVersion{desc.engineVersion[0], desc.engineVersion[1], desc.engineVersion[2]}
{
    @autoreleasepool
    {
#if defined(TARGET_OS_OSX)
        NSArray<id<MTLDevice>>* mtlDevices = [MTLCopyAllDevices() autorelease];
#else
        NSArray<id<MTLDevice>>* mtlDevices = @[ MTLCreateSystemDefaultDevice() ];
#endif
        for (id<MTLDevice> mtlDevice in mtlDevices)
            m_physicalDevices.push_back(ext::make_unique<MetalPhysicalDevice>([mtlDevice retain]));
    }
}

ext::unique_ptr<Surface> MetalInstance::createSurface(GLFWwindow* glfwWindow)
{
    return ext::make_unique<MetalSurface>(glfwWindow);
}

const ext::vector<PhysicalDevice*> MetalInstance::listPhysicalDevices()
{
    ext::vector<PhysicalDevice*> ret;
    for (auto& phyDevice : m_physicalDevices)
        ret.push_back(phyDevice.get());
    return ret;
}

ext::unique_ptr<Device> MetalInstance::newDevice(const Device::Descriptor& desc, const PhysicalDevice* phyDevice)
{
    const MetalPhysicalDevice* mtlPhyDevice;
    if (phyDevice != nullptr)
    {
        if (phyDevice->isSuitable(desc) == false)
            throw ext::runtime_error("device not suitable");
        mtlPhyDevice = dynamic_cast<const MetalPhysicalDevice*>(phyDevice);
    }
    else
    {
        mtlPhyDevice = findSuitableDevice(desc);
    }
    assert(mtlPhyDevice);
    return ext::make_unique<MetalDevice>(*mtlPhyDevice, desc);
}

MetalPhysicalDevice* MetalInstance::findSuitableDevice(const Device::Descriptor& desc)
{
    // TODO : better device selection
    ext::vector<PhysicalDevice*> phyDevices = listPhysicalDevices();
    for (auto& phyDevice : phyDevices)
    {
        if (phyDevice->isSuitable(desc) == false)
            continue;
        return dynamic_cast<MetalPhysicalDevice*>(phyDevice);
    }
    return nullptr;
}

} // namespace gfx
