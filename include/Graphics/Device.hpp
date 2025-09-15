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

#include "Graphics/QueueCapabilities.hpp"
#include "Graphics/Swapchain.hpp"
#include "Graphics/ShaderLib.hpp"
#include "Graphics/GraphicsPipeline.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/CommandBufferPool.hpp"
#include "Graphics/ParameterBlockPool.hpp"
#include "Graphics/CommandBuffer.hpp"
#include "Graphics/Enums.hpp"

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    #include <filesystem>
    #include <optional>
    #include <vector>
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

    virtual Backend backend() const = 0;

    virtual ext::unique_ptr<Swapchain> newSwapchain(const Swapchain::Descriptor&) const = 0;
    virtual ext::unique_ptr<ShaderLib> newShaderLib(const ext::filesystem::path&) const = 0;
    virtual ext::unique_ptr<GraphicsPipeline> newGraphicsPipeline(const GraphicsPipeline::Descriptor&) = 0;
    virtual ext::unique_ptr<Buffer> newBuffer(const Buffer::Descriptor&) const = 0;
    virtual ext::unique_ptr<Texture> newTexture(const Texture::Descriptor&) const = 0;
    virtual ext::unique_ptr<CommandBufferPool> newCommandBufferPool() const = 0;
    virtual ext::unique_ptr<ParameterBlockPool> newParameterBlockPool() const = 0;
    virtual ext::unique_ptr<Sampler> newSampler(const Sampler::Descriptor&) const = 0;

#if defined(GFX_IMGUI_ENABLED)
    virtual void imguiInit(ext::vector<PixelFormat> colorAttachmentPxFormats, ext::optional<PixelFormat> depthAttachmentPxFormat = ext::nullopt) const = 0;
    virtual void imguiNewFrame() const = 0;
    virtual void imguiShutdown() = 0;
#endif

    virtual void submitCommandBuffers(ext::unique_ptr<CommandBuffer>&&) = 0;
    virtual void submitCommandBuffers(ext::vector<ext::unique_ptr<CommandBuffer>>) = 0;

    virtual void waitCommandBuffer(const CommandBuffer*) = 0;
    virtual void waitIdle() = 0;

    virtual ~Device() = default;

protected:
    Device() = default;

public:
    Device& operator=(const Device&) = delete;
    Device& operator=(Device&&) = delete;
};

} // namespace gfx

#endif // DEVICE_HPP
