/*
 * ---------------------------------------------------
 * PhysicalDevice.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/29 10:33:15
 * ---------------------------------------------------
 */

#ifndef PHYSICALDEVICE_HPP
#define PHYSICALDEVICE_HPP

#include "Graphics/Device.hpp"

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <string>
    namespace ext = std;
#endif

namespace gfx
{

class PhysicalDevice
{
public:
    PhysicalDevice(const PhysicalDevice&) = delete;
    PhysicalDevice(PhysicalDevice&&) = delete;

    virtual ext::string name() const = 0;

    virtual bool isSuitable(const Device::Descriptor&) const = 0;

    virtual ~PhysicalDevice() = default;

protected:
    PhysicalDevice() = default;

public:
    PhysicalDevice& operator=(const PhysicalDevice&) = delete;
    PhysicalDevice& operator=(PhysicalDevice&&) = delete;
};

} // namespace gfx

#endif // PHYSICALDEVICE_HPP
