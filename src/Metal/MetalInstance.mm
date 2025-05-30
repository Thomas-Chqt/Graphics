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
}

ext::unique_ptr<Surface> MetalInstance::createSurface(GLFWwindow* glfwWindow) { @autoreleasepool
{
    return ext::make_unique<MetalSurface>(glfwWindow);
}}

ext::vector<ext::unique_ptr<PhysicalDevice>> MetalInstance::listPhysicalDevices() { @autoreleasepool
{
    ext::vector<ext::unique_ptr<PhysicalDevice>> physicalDevices;
#if defined(TARGET_OS_OSX)
    NSArray<id<MTLDevice>>* mtlDevices = [MTLCopyAllDevices() autorelease];
#else
    NSArray<id<MTLDevice>>* mtlDevices = @[MTLCreateSystemDefaultDevice()];
#endif
    for (id<MTLDevice> mtlDevice in mtlDevices)
        physicalDevices.push_back(ext::make_unique<MetalPhysicalDevice>([mtlDevice retain]));
    return physicalDevices;
}}

ext::unique_ptr<Device> MetalInstance::newDevice(const Device::Descriptor& desc, const PhysicalDevice& phyDevice) { @autoreleasepool
{
    if (phyDevice.isSuitable(desc) == false)
        throw ext::runtime_error("device not suitable");

    const MetalPhysicalDevice* metalPhyDevice = dynamic_cast<const MetalPhysicalDevice*>(&phyDevice);
    assert(metalPhyDevice);

    return ext::make_unique<MetalDevice>(*metalPhyDevice, desc);
}}

ext::unique_ptr<Device> MetalInstance::newDevice(const Device::Descriptor& desc)
{
    ext::unique_ptr<MetalPhysicalDevice> metalPhyDevice = findSuitableDevice(desc);
    if (metalPhyDevice == nullptr)
        throw ext::runtime_error("no suitable device found");

    return ext::make_unique<MetalDevice>(*metalPhyDevice, desc);
}

ext::unique_ptr<MetalPhysicalDevice> MetalInstance::findSuitableDevice(const Device::Descriptor& desc) { @autoreleasepool
{
    ext::vector<ext::unique_ptr<PhysicalDevice>> phyDevices = listPhysicalDevices();
    for (auto& phyDevice : phyDevices)
    {
        if (phyDevice->isSuitable(desc) == false)
            continue;
        if (MetalPhysicalDevice* metalPhyDevice = dynamic_cast<MetalPhysicalDevice*>(phyDevice.release()))
            return ext::unique_ptr<MetalPhysicalDevice>(metalPhyDevice);
    }
    return nullptr;
}}

} // namespace gfx
