/*
 * ---------------------------------------------------
 * MetalDevice.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/23 07:17:42
 * ---------------------------------------------------
 */

#ifndef METAL_DEVICE_HPP
#define METAL_DEVICE_HPP

#include "Graphics/Device.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
template<typename T>
using id = T*;
class MTLDevice;
class MTLCommandQueue;
#endif // __OBJC__

namespace gfx
{

class MetalDevice : public Device
{
public:
    MetalDevice() = delete;
    MetalDevice(const MetalDevice&) = delete;
    MetalDevice(MetalDevice&&) = delete;

    MetalDevice(const Device::Descriptor&);

    ~MetalDevice() = default;

private:
    id<MTLDevice> m_mtlDevice = nullptr;
    id<MTLCommandQueue> m_commandQueue = nullptr;

public:
    MetalDevice& operator=(const MetalDevice&) = delete;
    MetalDevice& operator=(MetalDevice&&) = delete;
};

} // namespace gfx

#endif // METAL_DEVICE_HPP
