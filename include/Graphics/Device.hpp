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
#include "Graphics/RenderPass.hpp"
#include "Graphics/Swapchain.hpp"
#include "Graphics/QueueCapabilities.hpp"

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    namespace ext = std;
#endif

namespace gfx
{

class Device
{
public:
    struct Descriptor
    {
        QueueCapabilities queueCaps;
    };

public:
    Device(const Device&) = delete;
    Device(Device&&) = delete;

    virtual ext::unique_ptr<RenderPass> newRenderPass(const RenderPass::Descriptor&) const = 0;
    virtual ext::unique_ptr<Swapchain> newSwapchain(const Swapchain::Descriptor&) const = 0;

    virtual void beginFrame(void) = 0;
 
    virtual CommandBuffer& commandBuffer(void) = 0;

    virtual void submitCommandBuffer(const CommandBuffer&) = 0;
    virtual void presentSwapchain(const Swapchain&) = 0;

    virtual void endFrame(void) = 0;

    virtual ~Device() = default;

protected:
    Device() = default;

public:
    Device& operator=(const Device&) = delete;
    Device& operator=(Device&&) = delete;
};

} // namespace gfx

#endif // DEVICE_HPP
