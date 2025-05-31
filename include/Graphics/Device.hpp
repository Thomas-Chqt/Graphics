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

#include "Graphics/Enums.hpp"
#include "Graphics/Surface.hpp"

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    #include <utility>
    #include <cstdint>
    namespace ext = std;
#endif

namespace gfx
{

struct QueueCapability
{
    QueueCapabilityFlag flags;
    ext::vector<Surface*> surfaceSupport;
};

class Device
{
public:
    struct Descriptor
    {
        ext::vector<ext::pair<QueueCapability, uint32_t>> queues;

        inline static Descriptor singleQueuePatern(Surface* s)
        {
            return Descriptor
            {
                .queues = {
                    ext::make_pair(gfx::QueueCapability {
                            .flags = QueueCapabilityFlag::Graphics |
                                     QueueCapabilityFlag::Compute  |
                                     QueueCapabilityFlag::Transfer,
                            .surfaceSupport = { s }
                    }, 1)
                }
            };
        }
    };

public:
    Device(const Device&) = delete;
    Device(Device&&) = delete;

    virtual ~Device() = default;

protected:
    Device() = default;

public:
    Device& operator=(const Device&) = delete;
    Device& operator=(Device&&) = delete;
};

} // namespace gfx

#endif // DEVICE_HPP
