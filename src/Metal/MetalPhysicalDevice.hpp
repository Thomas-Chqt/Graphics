/*
 * ---------------------------------------------------
 * MetalPhysicalDevice.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/30 12:46:04
 * ---------------------------------------------------
 */

#ifndef METALPHYSICALDEVICE_HPP
#define METALPHYSICALDEVICE_HPP

#include "Graphics/PhysicalDevice.hpp"
#include <string>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <string>
    namespace ext = std;
#endif

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    template<typename T>
    using id = T*;
    class MTLDevice;
#endif // __OBJC__

namespace gfx
{

class MetalPhysicalDevice : public PhysicalDevice
{
public:
    MetalPhysicalDevice(const MetalPhysicalDevice&) = delete;
    MetalPhysicalDevice(MetalPhysicalDevice&&) = delete;

    MetalPhysicalDevice(id<MTLDevice>);

    ext::string name() const override;

    bool isSuitable(const Device::Descriptor&) const override;

    inline id<MTLDevice> mtlDevice() const { return m_mtlDevice; }

    ~MetalPhysicalDevice();

private:
    id<MTLDevice> m_mtlDevice = nullptr;

public:
    MetalPhysicalDevice& operator=(const MetalPhysicalDevice&) = delete;
    MetalPhysicalDevice& operator=(MetalPhysicalDevice&&) = delete;
};

} // namespace gfx

#endif // METALPHYSICALDEVICE_HPP
