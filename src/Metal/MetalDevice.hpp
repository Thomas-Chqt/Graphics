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
#include "Graphics/CommandBuffer.hpp"
#include "Graphics/Drawable.hpp"
#include "Graphics/ParameterBlock.hpp"
#include "Graphics/Texture.hpp"

#include "Metal/MetalCommandBufferPool.hpp"
#include "Metal/MetalBuffer.hpp"
#include "Metal/MetalCommandBuffer.hpp"
#include "Metal/MetalParameterBlockPool.hpp"

namespace gfx
{

class MetalDevice : public Device
{
public:
    MetalDevice() = delete;
    MetalDevice(const MetalDevice&) = delete;
    MetalDevice(MetalDevice&&) = delete;

    MetalDevice(id<MTLDevice>&, const Device::Descriptor&);

    ext::unique_ptr<Swapchain> newSwapchain(const Swapchain::Descriptor&) const override;
    ext::unique_ptr<ShaderLib> newShaderLib(const ext::filesystem::path&) const override;
    ext::unique_ptr<GraphicsPipeline> newGraphicsPipeline(const GraphicsPipeline::Descriptor&) override;
    ext::unique_ptr<Buffer> newBuffer(const Buffer::Descriptor&) const override;
    ext::unique_ptr<Texture> newTexture(const Texture::Descriptor&) const override;

#if defined (GFX_IMGUI_ENABLED)
    void imguiInit(ext::vector<PixelFormat> colorAttachmentPxFormats, ext::optional<PixelFormat> depthAttachmentPxFormat) const override;
#endif

#if defined(GFX_IMGUI_ENABLED)
    void imguiNewFrame() const override;
#endif

    void beginFrame() override;
 
    ParameterBlock& parameterBlock(const ParameterBlock::Layout&) override;

    CommandBuffer& commandBuffer() override;

    void submitCommandBuffer(CommandBuffer&) override;
    void presentDrawable(const ext::shared_ptr<Drawable>&) override;

    void endFrame() override;

    void waitIdle() const override;

    inline Backend backend() const override { return Backend::metal; }
    inline uint32_t currentFrameIdx() const override { return static_cast<uint32_t>(ext::distance(m_frameDatas.begin(), PerFrameInFlight<FrameData>::const_iterator(m_currFrameData))); };

    inline const id<MTLDevice>& mtlDevice() const { return m_mtlDevice; }

#if defined(GFX_IMGUI_ENABLED)
    void imguiShutdown() const override;
#endif

    ~MetalDevice() override;

private:
    struct FrameData
    {
        MetalParameterBlockPool pBlockPool;
        MetalCommandBufferPool commandBufferPool;

        ext::vector<MetalCommandBuffer*> submittedCommandBuffers;
        MetalCommandBuffer* waitedCommandBuffer = nullptr;
    };

    id<MTLDevice> m_mtlDevice = nil;
    id<MTLCommandQueue> m_queue = nil;

    PerFrameInFlight<FrameData> m_frameDatas;
    PerFrameInFlight<FrameData>::iterator m_currFrameData = m_frameDatas.begin();

public:
    MetalDevice& operator=(const MetalDevice&) = delete;
    MetalDevice& operator=(MetalDevice&&) = delete;
};

} // namespace gfx

#endif // METAL_DEVICE_HPP
