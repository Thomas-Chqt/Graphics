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

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
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

    ext::unique_ptr<Swapchain> newSwapchain(const Swapchain::Descriptor&) const override;

    ext::unique_ptr<ShaderLib> newShaderLib(const ext::filesystem::path&) const override;

    void beginFrame(void) override;
 
    ext::unique_ptr<CommandBuffer> commandBuffer(void) override;

    void submitCommandBuffer(ext::unique_ptr<CommandBuffer>&&) override;
    void presentDrawable(const ext::shared_ptr<Drawable>&) override;

    void endFrame(void) override;

    void waitIdle(void) override;

    inline const id<MTLDevice>& mtlDevice(void) const { return m_mtlDevice; }

    ~MetalDevice();

private:
    id<MTLDevice> m_mtlDevice;
    id<MTLCommandQueue> m_queue;

    ext::vector<ext::unique_ptr<MetalCommandBuffer>> m_submittedCommandBuffers;

public:
    MetalDevice& operator=(const MetalDevice&) = delete;
    MetalDevice& operator=(MetalDevice&&) = delete;
};

} // namespace gfx

#endif // METAL_DEVICE_HPP
