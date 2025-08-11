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
#include "Metal/MetalGraphicsPipeline.hpp"
#include "Metal/MetalParameterBlockPool.hpp"
#include "Metal/MetalSwapchain.hpp"
#include "Metal/MetalCommandBuffer.hpp"
#include "Metal/MetalDrawable.hpp"
#include "Metal/MetalShaderLib.hpp"
#include "Metal/imgui_impl_metal.h"

#import "Metal/MetalEnums.h"

#include <Metal/Metal.h>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    #include <queue>
    #include <cassert>
    #include <cstdint>
    namespace ext = std;
#endif

namespace gfx
{

MetalDevice::MetalDevice(id<MTLDevice>& device, const Device::Descriptor& desc) { @autoreleasepool
{
    m_mtlDevice = [device retain];
    m_queue = [device newCommandQueue];

    m_frameDatas.reserve(desc.maxFrameInFlight);
    for (uint32_t i = 0; i < desc.maxFrameInFlight; i++)
        m_frameDatas.emplace_back(this);

    m_currFrameData = m_frameDatas.begin();
}}

ext::unique_ptr<Swapchain> MetalDevice::newSwapchain(const Swapchain::Descriptor& desc) const
{
    return ext::make_unique<MetalSwapchain>(*this, desc);
}

ext::unique_ptr<ShaderLib> MetalDevice::newShaderLib(const ext::filesystem::path& path) const
{
    return ext::make_unique<MetalShaderLib>(*this, path);
}

ext::unique_ptr<GraphicsPipeline> MetalDevice::newGraphicsPipeline(const GraphicsPipeline::Descriptor& desc)
{
    return ext::make_unique<MetalGraphicsPipeline>(*this, desc);
}

ext::unique_ptr<Buffer> MetalDevice::newBuffer(const Buffer::Descriptor& desc) const
{
    return ext::make_unique<MetalBuffer>(this, desc);
}

#if defined (GFX_IMGUI_ENABLED)
void MetalDevice::imguiInit(uint32_t, ext::vector<PixelFormat> colorPixelFomats, ext::optional<PixelFormat> depthPixelFormat) const { @autoreleasepool
{
    ImGui_ImplMetal_Init(m_mtlDevice, 1,
        toMTLPixelFormat(colorPixelFomats[0]),
        depthPixelFormat ? toMTLPixelFormat(depthPixelFormat.value()) : MTLPixelFormatInvalid,
        MTLPixelFormatInvalid);
}}
#endif

#if defined(GFX_IMGUI_ENABLED)
void MetalDevice::imguiNewFrame() const
{
    ImGui_ImplMetal_NewFrame();
}
#endif

void MetalDevice::beginFrame(void) { @autoreleasepool
{
    if (m_currFrameData->submittedCommandBuffers.empty() == false)
        [m_currFrameData->submittedCommandBuffers.back()->mtlCommandBuffer() waitUntilCompleted];
    m_currFrameData->reset();
}}

ParameterBlock& MetalDevice::parameterBlock(const ParameterBlock::Layout& pbLayout)
{
    return m_currFrameData->pBlockPool.get(pbLayout);
}

CommandBuffer& MetalDevice::commandBuffer(void)
{
    return m_currFrameData->commandBuffers.emplace_back(m_queue);
}

void MetalDevice::submitCommandBuffer(CommandBuffer& aCommandBuffer)
{
    MetalCommandBuffer* commandbuffer = dynamic_cast<MetalCommandBuffer*>(&aCommandBuffer);
    assert(commandbuffer);
    m_currFrameData->submittedCommandBuffers.push_back(commandbuffer);
}

void MetalDevice::presentDrawable(const ext::shared_ptr<Drawable>& _drawable) { @autoreleasepool
{
    ext::shared_ptr<MetalDrawable> drawable = ext::dynamic_pointer_cast<MetalDrawable>(_drawable);
    [m_currFrameData->submittedCommandBuffers.back()->mtlCommandBuffer() presentDrawable:drawable->mtlDrawable()];
}}

void MetalDevice::endFrame(void) { @autoreleasepool
{
    for (auto& buff : m_currFrameData->submittedCommandBuffers)
        [buff->mtlCommandBuffer() commit];
    m_currFrameData++;
    if (m_currFrameData == m_frameDatas.end())
        m_currFrameData = m_frameDatas.begin();
}}

void MetalDevice::waitIdle(void) const { @autoreleasepool
{
    for (auto& frameData : m_frameDatas) {
        if (frameData.submittedCommandBuffers.empty() == false)
            [frameData.submittedCommandBuffers.back()->mtlCommandBuffer() waitUntilCompleted];
    }
}}

#if defined(GFX_IMGUI_ENABLED)
void MetalDevice::imguiShutdown() const
{
    ImGui_ImplMetal_Shutdown();
}
#endif

MetalDevice::~MetalDevice() { @autoreleasepool
{
    waitIdle();
    m_frameDatas.clear();
    [m_queue release];
    [m_mtlDevice release];
}}

MetalDevice::FrameData::FrameData(const MetalDevice* device)
    : pBlockPool(device)
{
}

void MetalDevice::FrameData::reset()
{
    submittedCommandBuffers.clear();
    commandBuffers.clear();
    pBlockPool.reset();
}

}
