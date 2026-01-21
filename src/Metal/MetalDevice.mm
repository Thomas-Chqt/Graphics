/*
 * ---------------------------------------------------
 * MetalDevice.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/27 07:35:19
 * ---------------------------------------------------
 */

#include "Graphics/Device.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Swapchain.hpp"
#include "Graphics/CommandBuffer.hpp"
#include "Graphics/Drawable.hpp"
#include "Graphics/ParameterBlock.hpp"

#include "Metal/MetalDevice.hpp"
#include "Metal/MetalBuffer.hpp"
#include "Metal/MetalCommandBufferPool.hpp"
#include "Metal/MetalGraphicsPipeline.hpp"
#include "Metal/MetalParameterBlockPool.hpp"
#include "Metal/MetalSwapchain.hpp"
#include "Metal/MetalCommandBuffer.hpp"
#include "Metal/MetalDrawable.hpp"
#include "Metal/MetalShaderLib.hpp"
#include "MetalParameterBlockLayout.hpp"
#if defined(GFX_IMGUI_ENABLED)
# include "Metal/imgui_impl_metal.h"
#endif
#include "Metal/MetalTexture.hpp"
#include "Metal/MetalSampler.hpp"

#import "Metal/MetalEnums.hpp"

namespace gfx
{

MetalDevice::MetalDevice(id<MTLDevice> device, const Device::Descriptor&)
    : m_mtlDevice(device) { @autoreleasepool
{
    m_queue = [m_mtlDevice newCommandQueue];
    m_sharedEvent = [m_mtlDevice newSharedEvent];
#if defined (GFX_BUILD_TRACY) && (defined(__aarch64__) || defined(__arm64__))
    s_tracyMtlContext = TracyMetalContext(device);
#endif
}}

std::unique_ptr<Swapchain> MetalDevice::newSwapchain(const Swapchain::Descriptor& desc) const
{
    return std::make_unique<MetalSwapchain>(*this, desc);
}

std::unique_ptr<ShaderLib> MetalDevice::newShaderLib(const std::filesystem::path& path) const
{
    return std::make_unique<MetalShaderLib>(*this, path);
}

std::unique_ptr<ParameterBlockLayout> MetalDevice::newParameterBlockLayout(const ParameterBlockLayout::Descriptor& desc) const
{
    return std::make_unique<MetalParameterBlockLayout>(desc);
}

std::unique_ptr<GraphicsPipeline> MetalDevice::newGraphicsPipeline(const GraphicsPipeline::Descriptor& desc) const
{
    return std::make_unique<MetalGraphicsPipeline>(*this, desc);
}

std::unique_ptr<Buffer> MetalDevice::newBuffer(const Buffer::Descriptor& desc) const
{
    return std::make_unique<MetalBuffer>(*this, desc);
}

std::unique_ptr<Texture> MetalDevice::newTexture(const Texture::Descriptor& desc) const
{
    return std::make_unique<MetalTexture>(*this, desc);
}

std::unique_ptr<CommandBufferPool> MetalDevice::newCommandBufferPool() const
{
    return std::make_unique<MetalCommandBufferPool>(&m_queue);
}

std::unique_ptr<ParameterBlockPool> MetalDevice::newParameterBlockPool(const ParameterBlockPool::Descriptor& descriptor) const
{
    return std::make_unique<MetalParameterBlockPool>(this, descriptor);
}

std::unique_ptr<Sampler> MetalDevice::newSampler(const Sampler::Descriptor& desc) const
{
    return std::make_unique<MetalSampler>(*this, desc);
}

#if defined (GFX_IMGUI_ENABLED)
void MetalDevice::imguiInit(std::vector<PixelFormat> colorPixelFomats, std::optional<PixelFormat> depthPixelFormat) const { @autoreleasepool
{
    ImGui_ImplMetal_Init(
        m_mtlDevice,
        1,
        toMTLPixelFormat(colorPixelFomats.front()),
        depthPixelFormat.has_value() ? toMTLPixelFormat(*depthPixelFormat) : MTLPixelFormatInvalid,
        MTLPixelFormatInvalid
    );
}}

void MetalDevice::imguiNewFrame() const { @autoreleasepool
{
    ImGui_ImplMetal_NewFrame();
}}

void MetalDevice::imguiShutdown() { @autoreleasepool
{
    ImGui_ImplMetal_Shutdown();
}}
#endif

void MetalDevice::submitCommandBuffers(const std::shared_ptr<CommandBuffer>& aCommandBuffer) { @autoreleasepool // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
{
    std::scoped_lock lock(m_submitMtx);

    auto commandBuffer = std::dynamic_pointer_cast<MetalCommandBuffer>(aCommandBuffer);
    assert(commandBuffer);

    [commandBuffer->mtlCommandBuffer() encodeSignalEvent:m_sharedEvent value:m_nextSharedEventValue];
    commandBuffer->setSignaledSharedEventValue(m_nextSharedEventValue);
    m_nextSharedEventValue++;
    [commandBuffer->mtlCommandBuffer() commit];
    m_submittedCommandBuffers.push_back(commandBuffer);
}}

void MetalDevice::submitCommandBuffers(const std::vector<std::shared_ptr<CommandBuffer>>& commandBuffers)
{
    for (auto& commandBuffer : commandBuffers)
        submitCommandBuffers(commandBuffer);
}

void MetalDevice::waitCommandBuffer(const CommandBuffer& aCommandBuffer) { @autoreleasepool
{
    ZoneScoped;
    std::scoped_lock lock(m_submitMtx);

    auto waitedIt = std::ranges::find_if(m_submittedCommandBuffers, [&](auto& c){ return c.get() == &aCommandBuffer; });
    if (waitedIt != m_submittedCommandBuffers.end())
    {
        [m_sharedEvent waitUntilSignaledValue:(*waitedIt)->signaledSharedEventValue() timeoutMS:UINT64_MAX];
        TracyMetalCollect(s_tracyMtlContext);
        m_submittedCommandBuffers.erase(m_submittedCommandBuffers.begin(), std::next(waitedIt));
    }
}}

void MetalDevice::waitIdle()
{
    if (m_submittedCommandBuffers.empty() == false)
        waitCommandBuffer(*m_submittedCommandBuffers.back());
}

MetalDevice::~MetalDevice()
{
    waitIdle();
    TracyMetalDestroy(s_tracyMtlContext);
}

}
