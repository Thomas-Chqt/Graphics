/*
 * ---------------------------------------------------
 * MetalInstance.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/28 06:18:35
 * ---------------------------------------------------
 */

#include "Graphics/Device.hpp"
#include "Graphics/Instance.hpp"

#include "Metal/MetalInstance.hpp"
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
    namespace ext = std;
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

const ext::vector<Device::Info> MetalInstance::listAvailableDevices() { @autoreleasepool
{
    ext::vector<Device::Info> deviceInfos;
#if defined(TARGET_OS_OSX)
    NSArray<id<MTLDevice>>* mtlDevices = MTLCopyAllDevices();
#else
    NSArray<id<MTLDevice>>* mtlDevices = @[ MTLCreateSystemDefaultDevice() ];
#endif
    for (id<MTLDevice> mtlDevice in mtlDevices)
    {
        m_mtlDevices.push_back(mtlDevice);
        Device::Info deviceInfo = deviceInfos.emplace_back();
        deviceInfo.id = m_mtlDevices.size() - 1;
        deviceInfo.name = [mtlDevice.name UTF8String];
    }
    return deviceInfos;
}}

ext::unique_ptr<Device> MetalInstance::newDevice(const Device::Descriptor& desc)
{
    return ext::make_unique<MetalDevice>(desc);
}

} // namespace gfx
