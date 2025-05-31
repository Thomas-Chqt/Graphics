/*
 * ---------------------------------------------------
 * MetalDevice.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/27 07:35:19
 * ---------------------------------------------------
 */

#include "Metal/MetalDevice.hpp"

#include <Metal/Metal.h>

#if defined(GFX_USE_UTILSCPP)
#else
    #include <cassert>
#endif

namespace gfx
{

MetalDevice::MetalDevice(const MetalPhysicalDevice& phyDevice, const Device::Descriptor& desc) { @autoreleasepool
{
    assert(phyDevice.isSuitable(desc));
    m_mtlDevice = [phyDevice.mtlDevice() retain];
    for (auto& [_, count] : desc.queues)
        m_queues.push_back([m_mtlDevice newCommandQueue]);
}}

MetalDevice::~MetalDevice()
{
    for (auto& queue : m_queues)
        [queue release];
    [m_mtlDevice release];
}

}
