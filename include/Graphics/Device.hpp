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

#include "Graphics/CommandBuffer.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/RenderPass.hpp"
#include "Graphics/Surface.hpp"
#include "Graphics/Swapchain.hpp"

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    #include <utility>
    #include <cstdint>
    #include <memory>
    namespace ext = std;
#endif

namespace gfx
{

class PhysicalDevice;

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

    virtual const PhysicalDevice& physicalDevice(void) const = 0;

    virtual ext::unique_ptr<RenderPass> newRenderPass(const RenderPass::Descriptor&) const = 0;
    virtual ext::unique_ptr<Swapchain> newSwapchain(const Swapchain::Descriptor&) const = 0;

    virtual ext::unique_ptr<CommandBuffer> newCommandBuffer() = 0;

    virtual ~Device() = default;

protected:
    Device() = default;

public:
    Device& operator=(const Device&) = delete;
    Device& operator=(Device&&) = delete;
};

} // namespace gfx

#endif // DEVICE_HPP
