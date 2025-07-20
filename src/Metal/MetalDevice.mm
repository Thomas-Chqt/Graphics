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
#include "Metal/MetalBuffer.hpp"
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
    #include <queue>
    namespace ext = std;
#endif

namespace gfx
{

MetalDevice::MetalDevice(id<MTLDevice>& device, const Device::Descriptor& desc) : 
    m_frameData(desc.maxFrameInFlight), m_currFD(m_frameData.begin())
{
    @autoreleasepool {
        m_mtlDevice = [device retain];
        m_queue = [device newCommandQueue];
    }
}

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

ext::unique_ptr<Buffer> MetalDevice::newBuffer(const Buffer::Descriptor& desc) const
{
    return ext::make_unique<MetalBuffer>(*this, desc);
}

void MetalDevice::beginFrame(void) { @autoreleasepool
{
    if (m_currFD->submittedCommandBuffers.empty() == false)
        [m_currFD->submittedCommandBuffers.back()->mtlCommandBuffer() waitUntilCompleted];
    m_currFD->submittedCommandBuffers.clear();
    while (m_currFD->commandBuffers.empty() == false)
        m_currFD->commandBuffers.pop();
}}

CommandBuffer& MetalDevice::commandBuffer(void)
{
    m_currFD->commandBuffers.emplace(m_queue);
    return m_currFD->commandBuffers.back();
}

void MetalDevice::submitCommandBuffer(CommandBuffer& commandBuffer)
{
    assert(dynamic_cast<MetalCommandBuffer*>(&commandBuffer));
    m_currFD->submittedCommandBuffers.push_back(dynamic_cast<MetalCommandBuffer*>(&commandBuffer));
}

void MetalDevice::presentDrawable(const ext::shared_ptr<Drawable>& _drawable) { @autoreleasepool
{
    ext::shared_ptr<MetalDrawable> drawable = ext::dynamic_pointer_cast<MetalDrawable>(_drawable);
    [m_currFD->submittedCommandBuffers.back()->mtlCommandBuffer() presentDrawable:drawable->mtlDrawable()];
}}

void MetalDevice::endFrame(void) { @autoreleasepool
{
    for (auto& buff : m_currFD->submittedCommandBuffers)
        [buff->mtlCommandBuffer() commit];
    m_currFD++;
    if (m_currFD == m_frameData.end())
        m_currFD = m_frameData.begin();
}}

void MetalDevice::waitIdle(void) { @autoreleasepool
{
    for (auto& fd : m_frameData) {
        if (fd.submittedCommandBuffers.empty() == false)
            [fd.submittedCommandBuffers.back()->mtlCommandBuffer() waitUntilCompleted];
    }
}}

MetalDevice::~MetalDevice() { @autoreleasepool
{
    waitIdle();
    [m_queue release];
    [m_mtlDevice release];
}}

}
