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
#include "Metal/imgui_impl_metal.hpp"
#include "Metal/MetalTexture.hpp"
#include "Metal/MetalSampler.hpp"

#import "Metal/MetalEnums.h"

namespace gfx
{

MetalDevice::MetalDevice(id<MTLDevice>& device, const Device::Descriptor&) { @autoreleasepool
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

ext::unique_ptr<GraphicsPipeline> MetalDevice::newGraphicsPipeline(const GraphicsPipeline::Descriptor& desc)
{
    return ext::make_unique<MetalGraphicsPipeline>(*this, desc);
}

ext::unique_ptr<Buffer> MetalDevice::newBuffer(const Buffer::Descriptor& desc) const
{
    return ext::make_unique<MetalBuffer>(*this, desc);
}

ext::unique_ptr<Texture> MetalDevice::newTexture(const Texture::Descriptor& desc) const
{
    return ext::make_unique<MetalTexture>(*this, desc);
}

ext::unique_ptr<CommandBufferPool> MetalDevice::newCommandBufferPool() const
{
    return ext::make_unique<MetalCommandBufferPool>(&m_queue);
}

ext::unique_ptr<ParameterBlockPool> MetalDevice::newParameterBlockPool() const
{
    return ext::make_unique<MetalParameterBlockPool>(this);
}

ext::unique_ptr<Sampler> MetalDevice::newSampler(const Sampler::Descriptor& desc) const
{
    return ext::make_unique<MetalSampler>(*this, desc);
}

#if defined (GFX_IMGUI_ENABLED)
void MetalDevice::imguiInit(ext::vector<PixelFormat> colorPixelFomats, ext::optional<PixelFormat> depthPixelFormat) const
{
    ImGui_ImplMetal_Init(this, colorPixelFomats, depthPixelFormat);
}

void MetalDevice::imguiNewFrame() const
{
    ImGui_ImplMetal_NewFrame();
}

void MetalDevice::imguiShutdown()
{
    ImGui_ImplMetal_Shutdown();
}
#endif

void MetalDevice::submitCommandBuffers(ext::unique_ptr<CommandBuffer>&& aCommandBuffer) { @autoreleasepool // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
{
    auto* commandBuffer = dynamic_cast<MetalCommandBuffer*>(aCommandBuffer.release());
    assert(commandBuffer);

    [commandBuffer->mtlCommandBuffer() commit];
    m_submittedCommandBuffers.push_back(ext::unique_ptr<MetalCommandBuffer>(commandBuffer));
}}

void MetalDevice::submitCommandBuffers(ext::vector<ext::unique_ptr<CommandBuffer>> commandBuffers)
{
    for (auto& commandBuffer : commandBuffers)
        submitCommandBuffers(ext::move(commandBuffer));
}

void MetalDevice::waitCommandBuffer(const CommandBuffer* aCommandBuffer) { @autoreleasepool
{
    auto it = ext::ranges::find_if(m_submittedCommandBuffers, [&](auto& c){ return c.get() == aCommandBuffer; });
    if (it != m_submittedCommandBuffers.end())
    {
        [(*it)->mtlCommandBuffer() waitUntilCompleted];
        ++it;
        for(auto curr = m_submittedCommandBuffers.begin(); curr != it; ++curr) {
            if ((*curr)->pool())
                (*curr)->pool()->release(ext::move(*curr));
        }
        m_submittedCommandBuffers.erase(m_submittedCommandBuffers.begin(), it);
    }
}}

void MetalDevice::waitIdle()
{
    if (m_submittedCommandBuffers.empty() == false)
        waitCommandBuffer(m_submittedCommandBuffers.back().get());
}

MetalDevice::~MetalDevice() { @autoreleasepool
{
    waitIdle();
    [m_queue release];
    [m_mtlDevice release];
}}

}
