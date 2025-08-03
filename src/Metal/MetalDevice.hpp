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

#include "Metal/MetalCommandBuffer.hpp"
#include <memory>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    namespace ext = std;
    #include <vector>
    #include <queue>
#endif

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    template<typename T>
    using id = T*;
    class MTLDevice;
    class MTLCommandQueue;
#endif // __OBJC__

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
    ext::unique_ptr<GraphicsPipeline> newGraphicsPipeline(const GraphicsPipeline::Descriptor&) const override;
    ext::unique_ptr<Buffer> newBuffer(const Buffer::Descriptor&) const override;

#if defined (GFX_IMGUI_ENABLED)
    void imguiInit(uint32_t imageCount,
                   ext::vector<PixelFormat> colorAttachmentPxFormats,
                   ext::optional<PixelFormat> depthAttachmentPxFormat) const override;
#endif

#if defined(GFX_IMGUI_ENABLED)
    void imguiNewFrame() const override;
#endif

    void beginFrame(void) override;
 
    CommandBuffer& commandBuffer(void) override;

    void submitCommandBuffer(CommandBuffer&) override;
    void presentDrawable(const ext::shared_ptr<Drawable>&) override;

    void endFrame(void) override;

    void waitIdle(void) override;

    inline Backend backend() const override { return Backend::metal; }
    inline uint32_t maxFrameInFlight(void) const override { return m_frameDatas.size(); };
    inline uint32_t currentFrameIdx() const override { return ext::distance(m_frameDatas.begin(), ext::vector<FrameData>::const_iterator(m_currFrameData)); };

    inline const id<MTLDevice>& mtlDevice(void) const { return m_mtlDevice; }

#if defined(GFX_IMGUI_ENABLED)
    virtual void imguiShutdown() const override;
#endif

    ~MetalDevice();

private:
    struct FrameData
    {
        ext::queue<MetalCommandBuffer> commandBuffers;
        ext::vector<MetalCommandBuffer*> submittedCommandBuffers;
    };

    id<MTLDevice> m_mtlDevice;
    id<MTLCommandQueue> m_queue;

    ext::vector<FrameData> m_frameDatas;
    ext::vector<FrameData>::iterator m_currFrameData;

public:
    MetalDevice& operator=(const MetalDevice&) = delete;
    MetalDevice& operator=(MetalDevice&&) = delete;
};

} // namespace gfx

#endif // METAL_DEVICE_HPP
