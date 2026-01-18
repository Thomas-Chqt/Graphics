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
{
}

#if defined(GFX_GLFW_ENABLED)
std::unique_ptr<Surface> MetalInstance::createSurface(GLFWwindow* glfwWindow)
{
    return std::make_unique<MetalSurface>(glfwWindow);
}
#endif

std::unique_ptr<Device> MetalInstance::newDevice(const Device::Descriptor& desc) { @autoreleasepool
{
#if defined(TARGET_OS_OSX)
    NSArray<id<MTLDevice>>* mtlDevices = MTLCopyAllDevices();
#else
    NSArray<id<MTLDevice>>* mtlDevices = @[ MTLCreateSystemDefaultDevice() ];
#endif

    return std::make_unique<MetalDevice>(mtlDevices.firstObject, desc);
}}

} // namespace gfx
