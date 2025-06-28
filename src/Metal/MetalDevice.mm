/*
 * ---------------------------------------------------
 * MetalDevice.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/27 07:35:19
 * ---------------------------------------------------
 */

#include "Graphics/Device.hpp"
#include "Graphics/Swapchain.hpp"
#include "Graphics/CommandBuffer.hpp"
#include "Graphics/Drawable.hpp"

#include "Metal/MetalDevice.hpp"
#include "Metal/MetalGraphicsPipeline.hpp"
#include "Metal/MetalSwapchain.hpp"
#include "Metal/MetalCommandBuffer.hpp"
#include "Metal/MetalDrawable.hpp"
#include "Metal/MetalShaderLib.hpp"

#include <Metal/Metal.h>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    namespace ext = std;
#endif

namespace gfx
{

MetalDevice::MetalDevice(id<MTLDevice>& device) { @autoreleasepool
{
    m_mtlDevice = [device retain];
    m_queue = [device newCommandQueue];
}}

ext::unique_ptr<Swapchain> MetalDevice::newSwapchain(const Swapchain::Descriptor& desc) const
{
    return ext::make_unique<MetalSwapchain>(*this, desc);
}

ext::unique_ptr<ShaderLib> MetalDevice::newShaderLib(const ext::filesystem::path& path) const
{
    return ext::make_unique<MetalShaderLib>(*this, path);
}

ext::unique_ptr<GraphicsPipeline> MetalDevice::newGraphicsPipeline(const GraphicsPipeline::Descriptor& desc) const
{
    return ext::make_unique<MetalGraphicsPipeline>(*this, desc);
}

void MetalDevice::beginFrame(void) { @autoreleasepool
{
    if (m_submittedCommandBuffers.empty() == false)
        [m_submittedCommandBuffers.back()->mtlCommandBuffer() waitUntilCompleted];
    m_submittedCommandBuffers.clear();
}}

ext::unique_ptr<CommandBuffer> MetalDevice::commandBuffer(void)
{
    return ext::make_unique<MetalCommandBuffer>(m_queue);
}

void MetalDevice::submitCommandBuffer(ext::unique_ptr<CommandBuffer>&& commandBuffer)
{
    assert(dynamic_cast<MetalCommandBuffer*>(commandBuffer.get()));
    m_submittedCommandBuffers.push_back(ext::unique_ptr<MetalCommandBuffer>(dynamic_cast<MetalCommandBuffer*>(commandBuffer.release())));
}

void MetalDevice::presentDrawable(const ext::shared_ptr<Drawable>& _drawable) { @autoreleasepool
{
    ext::shared_ptr<MetalDrawable> drawable = ext::dynamic_pointer_cast<MetalDrawable>(_drawable);
    [m_submittedCommandBuffers.back()->mtlCommandBuffer() presentDrawable:drawable->mtlDrawable()];
}}

void MetalDevice::endFrame(void) { @autoreleasepool
{
    for (auto& buff : m_submittedCommandBuffers)
        [buff->mtlCommandBuffer() commit];
}}

void MetalDevice::waitIdle(void) { @autoreleasepool
{
    for (auto& buff : m_submittedCommandBuffers)
        [buff->mtlCommandBuffer() waitUntilCompleted];
}}

MetalDevice::~MetalDevice() { @autoreleasepool
{
    [m_queue release];
    [m_mtlDevice release];
}}

}
