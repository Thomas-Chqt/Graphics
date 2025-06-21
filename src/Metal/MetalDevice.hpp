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
#include "Graphics/RenderPass.hpp"
#include "Graphics/Swapchain.hpp"
#include "Graphics/CommandBuffer.hpp"

#include "Metal/MetalCommandBuffer.hpp"

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <mutex>
    namespace ext = std;
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

    MetalDevice(id<MTLDevice>&);

    ext::unique_ptr<RenderPass> newRenderPass(const RenderPass::Descriptor&) const override;
    ext::unique_ptr<Swapchain> newSwapchain(const Swapchain::Descriptor&) const override;

    void beginFrame(void) override;
 
    CommandBuffer& commandBuffer(void) override;

    void submitCommandBuffer(const CommandBuffer&) override;
    void presentSwapchain(const Swapchain&) override;

    void endFrame(void) override;

    const id<MTLDevice>& mtlDevice(void) const { return m_mtlDevice; }

    ~MetalDevice();

private:
    id<MTLDevice> m_mtlDevice;
    id<MTLCommandQueue> m_queue;
    ext::mutex m_queueMtx;

    MetalCommandBuffer m_commandBuffer;

public:
    MetalDevice& operator=(const MetalDevice&) = delete;
    MetalDevice& operator=(MetalDevice&&) = delete;
};

} // namespace gfx

#endif // METAL_DEVICE_HPP
