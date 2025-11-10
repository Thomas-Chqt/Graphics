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
#include "Graphics/Swapchain.hpp"
#include "Graphics/ShaderLib.hpp"
#include "Graphics/GraphicsPipeline.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/CommandBufferPool.hpp"
#include "Graphics/ParameterBlockPool.hpp"
#include "Graphics/CommandBuffer.hpp"
#include "Graphics/Enums.hpp"

#include "Metal/MetalCommandBuffer.hpp"

namespace gfx
{

class MetalDevice : public Device
{
public:
    MetalDevice() = delete;
    MetalDevice(const MetalDevice&) = delete;
    MetalDevice(MetalDevice&&) = delete;

    MetalDevice(id<MTLDevice>&, const Device::Descriptor&);

    inline Backend backend() const override { return Backend::metal; }

    std::unique_ptr<Swapchain> newSwapchain(const Swapchain::Descriptor&) const override;
    std::unique_ptr<ShaderLib> newShaderLib(const std::filesystem::path&) const override;
    std::unique_ptr<GraphicsPipeline> newGraphicsPipeline(const GraphicsPipeline::Descriptor&) override;
    std::unique_ptr<Buffer> newBuffer(const Buffer::Descriptor&) const override;
    std::unique_ptr<Texture> newTexture(const Texture::Descriptor&) const override;
    std::unique_ptr<CommandBufferPool> newCommandBufferPool() const override;
    std::unique_ptr<ParameterBlockPool> newParameterBlockPool(const ParameterBlockPool::Descriptor&) const override;
    std::unique_ptr<Sampler> newSampler(const Sampler::Descriptor&) const override;

#if defined (GFX_IMGUI_ENABLED)
    void imguiInit(std::vector<PixelFormat> colorAttachmentPxFormats, std::optional<PixelFormat> depthAttachmentPxFormat) const override;
    void imguiNewFrame() const override;
    void imguiShutdown() override;
#endif

    void submitCommandBuffers(std::unique_ptr<CommandBuffer>&&) override;
    void submitCommandBuffers(std::vector<std::unique_ptr<CommandBuffer>>) override;

    void waitCommandBuffer(const CommandBuffer*) override;
    void waitIdle() override;

    inline const id<MTLDevice>& mtlDevice() const { return m_mtlDevice; }

    ~MetalDevice() override;

private:
    id<MTLDevice> m_mtlDevice = nil;
    id<MTLCommandQueue> m_queue = nil;
    std::deque<std::unique_ptr<MetalCommandBuffer>> m_submittedCommandBuffers;
    std::mutex m_submitMtx;

public:
    MetalDevice& operator=(const MetalDevice&) = delete;
    MetalDevice& operator=(MetalDevice&&) = delete;
};

} // namespace gfx

#endif // METAL_DEVICE_HPP
