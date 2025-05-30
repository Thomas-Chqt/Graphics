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

namespace gfx
{

class Device
{
public:
    struct Descriptor
    {
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
