/*
 * ---------------------------------------------------
 * MetalPhysicalDevice.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/30 12:49:25
 * ---------------------------------------------------
 */

#include "Metal/MetalPhysicalDevice.hpp"

#import <Metal/Metal.h>

namespace gfx
{

MetalPhysicalDevice::MetalPhysicalDevice(id<MTLDevice> mtlDevice) : m_mtlDevice(mtlDevice)
{
}

ext::string MetalPhysicalDevice::name() const
{
    return [m_mtlDevice.name cStringUsingEncoding:NSUTF8StringEncoding];
}

bool MetalPhysicalDevice::isSuitable(const Device::Descriptor&) const
{
    return true;
}

MetalPhysicalDevice::~MetalPhysicalDevice()
{
    [m_mtlDevice release];
}

}
