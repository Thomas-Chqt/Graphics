/*
 * ---------------------------------------------------
 * Device.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/21 11:38:59
 * ---------------------------------------------------
 */

#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <cstdint>

#if defined(GFX_USE_UTILSCPP)
    #include "UtilsCPP/memory.hpp"
    namespace ext = utl;
#else
    #include <string>
    namespace ext = std;
#endif

namespace gfx
{

class Device
{
public:
    struct Descriptor
    {
        //
    };

    struct Info
    {
        uint32_t id;
        ext::string name;
    };

public:
    Device(const Device&) = delete;
    Device(Device&&)      = delete;

    virtual ~Device() = default;

protected:
    Device() = default;
    
public:
    Device& operator = (const Device&) = delete;
    Device& operator = (Device&&)      = delete;
};

}

#endif // DEVICE_HPP
