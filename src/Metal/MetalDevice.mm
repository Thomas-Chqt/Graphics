/*
 * ---------------------------------------------------
 * MetalDevice.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/27 07:35:19
 * ---------------------------------------------------
 */

#include "Graphics/Device.hpp"
#include "Graphics/RenderPass.hpp"
#include "Graphics/Swapchain.hpp"
#include "Graphics/CommandBuffer.hpp"

#include "Metal/MetalDevice.hpp"
#include "Metal/MetalRenderPass.hpp"
#include "Metal/MetalSwapchain.hpp"
#include "Metal/MetalCommandBuffer.hpp"

#include <Metal/Metal.h>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    #include <mutex>
    namespace ext = std;
#endif

namespace gfx
{

MetalDevice::MetalDevice(id<MTLDevice>& device) { @autoreleasepool
{
    m_mtlDevice = [device retain];
    m_queue = [device newCommandQueue];
}}

ext::unique_ptr<RenderPass> MetalDevice::newRenderPass(const RenderPass::Descriptor& desc) const
{
    return ext::make_unique<MetalRenderPass>(desc);
}

ext::unique_ptr<Swapchain> MetalDevice::newSwapchain(const Swapchain::Descriptor& desc) const
{
    return ext::make_unique<MetalSwapchain>(*this, desc);
}

ext::unique_ptr<CommandBuffer> MetalDevice::newCommandBuffer()
{
    ext::lock_guard<ext::mutex> lock(m_queueMtx);
    return ext::make_unique<MetalCommandBuffer>(m_queue);
}

MetalDevice::~MetalDevice() { @autoreleasepool
{
    [m_queue release];
    [m_mtlDevice release];
}}

}
