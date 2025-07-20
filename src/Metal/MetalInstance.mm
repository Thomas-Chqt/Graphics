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

#import <Metal/Metal.h>
#import <TargetConditionals.h>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    namespace ext = std;
#endif

#if defined(GFX_GLFW_ENABLED)
    struct GLFWwindow;
#endif

namespace gfx
{

MetalInstance::MetalInstance(const Instance::Descriptor& _)
{
}

#if defined(GFX_GLFW_ENABLED)
ext::unique_ptr<Surface> MetalInstance::createSurface(GLFWwindow* glfwWindow)
{
    return ext::make_unique<MetalSurface>(glfwWindow);
}
#endif

ext::unique_ptr<Device> MetalInstance::newDevice(const Device::Descriptor& desc) { @autoreleasepool
{
#if defined(TARGET_OS_OSX)
    NSArray<id<MTLDevice>>* mtlDevices = [MTLCopyAllDevices() autorelease];
#else
    NSArray<id<MTLDevice>>* mtlDevices = @[ MTLCreateSystemDefaultDevice() ];
#endif
    for (id<MTLDevice> mtlDevice in mtlDevices)
        m_devices.push_back([mtlDevice retain]);

    return ext::make_unique<MetalDevice>(m_devices.front(), desc);
}}

} // namespace gfx
