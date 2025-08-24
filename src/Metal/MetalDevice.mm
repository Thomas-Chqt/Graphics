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
#include "Metal/imgui_impl_metal.h"
#include "Metal/MetalTexture.hpp"

#import "Metal/MetalEnums.h"

#import <Metal/Metal.h>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    #include <queue>
    #include <cassert>
    #include <cstdint>
    #include <iterator>
    #include <algorithm>
    namespace ext = std;
#endif

namespace gfx
{

MetalDevice::MetalDevice(id<MTLDevice>& device, const Device::Descriptor&) { @autoreleasepool
{
    m_mtlDevice = [device retain];
    m_queue = [device newCommandQueue];

    for (auto& frameData : m_frameDatas) {
        frameData.pBlockPool = MetalParameterBlockPool(this);
        frameData.commandBufferPool = MetalCommandBufferPool(&m_queue);
    }
}}

ext::unique_ptr<Swapchain> MetalDevice::newSwapchain(const Swapchain::Descriptor& desc) const
{
    return ext::make_unique<MetalSwapchain>(this, desc);
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

ext::unique_ptr<Texture> MetalDevice::newTexture(const Texture::Descriptor& desc) const
{
    return ext::make_unique<MetalTexture>(this, desc);

}

#if defined (GFX_IMGUI_ENABLED)
void MetalDevice::imguiInit(ext::vector<PixelFormat> colorPixelFomats, ext::optional<PixelFormat> depthPixelFormat) const { @autoreleasepool
{
    ImGui_ImplMetal_Init(m_mtlDevice, 1,
        toMTLPixelFormat(colorPixelFomats[0]),
        depthPixelFormat ? toMTLPixelFormat(depthPixelFormat.value()) : MTLPixelFormatInvalid,
        MTLPixelFormatInvalid);
}}
#endif

#if defined(GFX_IMGUI_ENABLED)
void MetalDevice::imguiNewFrame() const { @autoreleasepool
{
    ImGui_ImplMetal_NewFrame();
}}
#endif

void MetalDevice::beginFrame() { @autoreleasepool
{
    if (m_currFrameData->waitedCommandBuffer != nullptr)
    {
        [m_currFrameData->waitedCommandBuffer->mtlCommandBuffer() waitUntilCompleted];
        auto it = ext::ranges::find(m_currFrameData->submittedCommandBuffers, m_currFrameData->waitedCommandBuffer);
        auto upBound = it == m_currFrameData->submittedCommandBuffers.end() ? it : ext::next(it);
        m_currFrameData->submittedCommandBuffers.erase(m_currFrameData->submittedCommandBuffers.begin(), upBound);
        m_currFrameData->waitedCommandBuffer = nullptr;
    }

    m_currFrameData->commandBufferPool.reset();
    m_currFrameData->pBlockPool.reset();
}}

ParameterBlock& MetalDevice::parameterBlock(const ParameterBlock::Layout& pbLayout)
{
    return m_currFrameData->pBlockPool.get(pbLayout);
}

CommandBuffer& MetalDevice::commandBuffer()
{
    return m_currFrameData->commandBufferPool.get();
}

void MetalDevice::submitCommandBuffer(CommandBuffer& aCommandBuffer)
{
    auto* commandbuffer = dynamic_cast<MetalCommandBuffer*>(&aCommandBuffer);
    assert(commandbuffer);
    m_currFrameData->submittedCommandBuffers.push_back(commandbuffer);
}

void MetalDevice::presentDrawable(const ext::shared_ptr<Drawable>& _drawable) { @autoreleasepool
{
    ext::shared_ptr<MetalDrawable> drawable = ext::dynamic_pointer_cast<MetalDrawable>(_drawable);
    [m_currFrameData->submittedCommandBuffers.back()->mtlCommandBuffer() presentDrawable:drawable->mtlDrawable()];
    // TODO : keep strong ref to drawable
}}

void MetalDevice::endFrame() { @autoreleasepool
{
    for (auto& buff : m_currFrameData->submittedCommandBuffers)
        [buff->mtlCommandBuffer() commit];

    if (m_currFrameData->submittedCommandBuffers.empty() == false)
        m_currFrameData->waitedCommandBuffer = m_currFrameData->submittedCommandBuffers.back();

    m_currFrameData->commandBufferPool.swapPools();
    m_currFrameData++;
    if (m_currFrameData == m_frameDatas.end())
        m_currFrameData = m_frameDatas.begin();
}}

void MetalDevice::waitIdle() const { @autoreleasepool
{
    for (auto& frameData : m_frameDatas) {
        if (frameData.waitedCommandBuffer != nullptr) {
            [frameData.waitedCommandBuffer->mtlCommandBuffer() waitUntilCompleted];
            //auto it = ext::ranges::find(frameData.submittedCommandBuffers, frameData.waitedCommandBuffer);
            //if (it != frameData.submittedCommandBuffers.end())
            //    frameData.submittedCommandBuffers.erase(frameData.submittedCommandBuffers.begin(), ext::next(it));
            //frameData.waitedCommandBuffer = nullptr;
        }
    }
}}

#if defined(GFX_IMGUI_ENABLED)
void MetalDevice::imguiShutdown() const { @autoreleasepool
{
    ImGui_ImplMetal_Shutdown();
}}
#endif

MetalDevice::~MetalDevice() { @autoreleasepool
{
    waitIdle();
    for (auto& frameData : m_frameDatas) {
        frameData.waitedCommandBuffer = nullptr;
        frameData.submittedCommandBuffers.clear();
        frameData.commandBufferPool.clear();
        frameData.pBlockPool.clear();
    }
    [m_queue release];
    [m_mtlDevice release];
}}

}
