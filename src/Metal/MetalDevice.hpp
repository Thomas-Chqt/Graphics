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
#include "Metal/MetalPhysicalDevice.hpp"

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
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

    MetalDevice(const MetalPhysicalDevice&, const Device::Descriptor&);

    inline const MetalPhysicalDevice& physicalDevice(void) const override { return *m_physicalDevice; }

    ext::unique_ptr<RenderPass> newRenderPass(const RenderPass::Descriptor&) const override;
    ext::unique_ptr<Swapchain> newSwapchain(const Swapchain::Descriptor&) const override;

    ext::unique_ptr<CommandBuffer> newCommandBuffer() override;

    const id<MTLDevice>& mtlDevice(void) const { return m_mtlDevice; }

    ~MetalDevice();

private:
    const MetalPhysicalDevice* m_physicalDevice;
    id<MTLDevice> m_mtlDevice;

    ext::vector<id<MTLCommandQueue>> m_queues;
    ext::mutex m_commandPoolsMutex;

public:
    MetalDevice& operator=(const MetalDevice&) = delete;
    MetalDevice& operator=(MetalDevice&&) = delete;
};

} // namespace gfx

#endif // METAL_DEVICE_HPP
