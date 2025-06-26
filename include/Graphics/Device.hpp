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
#include "Graphics/Drawable.hpp"
#include "Graphics/Swapchain.hpp"
#include "Graphics/QueueCapabilities.hpp"
#include "Graphics/ShaderLib.hpp"

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    #include <filesystem>
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

    virtual ext::unique_ptr<Swapchain> newSwapchain(const Swapchain::Descriptor&) const = 0;
    virtual ext::unique_ptr<ShaderLib> newShaderLib(const ext::filesystem::path&) const = 0;

    virtual void beginFrame(void) = 0;
 
    virtual ext::unique_ptr<CommandBuffer> commandBuffer(void) = 0;

    virtual void submitCommandBuffer(ext::unique_ptr<CommandBuffer>&&) = 0;
    virtual void presentDrawable(const ext::shared_ptr<Drawable>&) = 0;

    virtual void endFrame(void) = 0;

    virtual void waitIdle(void) = 0;

    virtual ~Device() = default;

protected:
    Device() = default;

public:
    Device& operator=(const Device&) = delete;
    Device& operator=(Device&&) = delete;
};

} // namespace gfx

#endif // DEVICE_HPP
