/*
 * ---------------------------------------------------
 * MetalInstance.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/28 06:18:35
 * ---------------------------------------------------
 */

#include "Graphics/Device.hpp"

#include "Metal/MetalInstance.hpp"
#include "Metal/MetalSurface.hpp"
#include "Metal/MetalDevice.hpp"

namespace gfx
{

MetalInstance::MetalInstance(const Instance::Descriptor&)
    : m_autoReleasePool([[NSAutoreleasePool alloc] init])
{
}

#if defined(GFX_GLFW_ENABLED)
ext::unique_ptr<Surface> MetalInstance::createSurface(GLFWwindow* glfwWindow)
{
    return ext::make_unique<MetalSurface>(glfwWindow);
}
#endif

ext::unique_ptr<Device> MetalInstance::newDevice(const Device::Descriptor& desc)
{
    @autoreleasepool
    {
        for (auto& device : m_devices)
            [device release]; // refresh the list when newDevice is called

#if defined(TARGET_OS_OSX)
        NSArray<id<MTLDevice>>* mtlDevices = [MTLCopyAllDevices() autorelease];
#else
        NSArray<id<MTLDevice>>* mtlDevices = @[ MTLCreateSystemDefaultDevice() ];
#endif

        for (NSUInteger i = 0; i < mtlDevices.count; i++)
            m_devices.push_back([[mtlDevices objectAtIndex:i] retain]);

        return ext::make_unique<MetalDevice>(m_devices.front(), desc);
    }
}

MetalInstance::~MetalInstance()
{
    for (auto& device : m_devices)
        [device release];
    [m_autoReleasePool drain];
}

} // namespace gfx
