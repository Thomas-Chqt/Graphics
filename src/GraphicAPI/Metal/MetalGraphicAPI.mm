/*
 * ---------------------------------------------------
 * MetalGraphicAPI.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/19 20:32:51
 * ---------------------------------------------------
 */

#include "GraphicAPI/Metal/MetalGraphicAPI.hpp"

#include "GraphicAPI/Metal/MetalBuffer.hpp"
#include "GraphicAPI/Metal/MetalFrameBuffer.hpp"
#include "GraphicAPI/Metal/MetalShader.hpp"
#include "GraphicAPI/Metal/MetalTexture.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/FrameBuffer.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "GraphicAPI/Metal/MetalGraphicPipeline.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/Sampler.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Texture.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/Types.hpp"
#include "Window/MetalWindow.hpp"
#include <Metal/Metal.h>
#include <cassert>
#include "Graphics/Error.hpp"
#include "GraphicAPI/Metal/MetalSampler.hpp"

#ifdef GFX_BUILD_IMGUI
    #include <imgui.h>
    #include <imgui_impl_metal.h>
#endif

namespace gfx
{

MetalGraphicAPI::MetalGraphicAPI(const utils::SharedPtr<Window>& window)
    : m_window(window.forceDynamicCast<MetalWindow>()) { @autoreleasepool
{
    m_mtlDevice = MTLCreateSystemDefaultDevice();
    assert(m_mtlDevice); // TODO use throw

    m_window->setGraphicAPI(this);
    
    m_commandQueue = [m_mtlDevice newCommandQueue];
    assert(m_commandQueue); // TODO use throw
}}

utils::SharedPtr<Shader> MetalGraphicAPI::newShader(const Shader::Descriptor& shaderDescriptor) const
{
    return utils::makeShared<MetalShader>(m_mtlDevice, shaderDescriptor).staticCast<Shader>();
}

utils::SharedPtr<GraphicPipeline> MetalGraphicAPI::newGraphicsPipeline(const GraphicPipeline::Descriptor& descriptor) const
{
    return utils::makeShared<MetalGraphicPipeline>(m_mtlDevice, descriptor).staticCast<GraphicPipeline>();
}

utils::SharedPtr<Buffer> MetalGraphicAPI::newBuffer(const Buffer::Descriptor& descriptor) const
{
    return utils::makeShared<MetalBuffer>(m_mtlDevice, descriptor).staticCast<Buffer>();
}

utils::SharedPtr<Texture> MetalGraphicAPI::newTexture(const Texture::Descriptor& descriptor) const
{
    return utils::makeShared<MetalTexture>(m_mtlDevice, descriptor).staticCast<Texture>();
}

utils::SharedPtr<Sampler> MetalGraphicAPI::newSampler(const Sampler::Descriptor& descriptor) const
{
    return utils::makeShared<MetalSampler>(m_mtlDevice, descriptor).staticCast<Sampler>();
}

utils::SharedPtr<FrameBuffer> MetalGraphicAPI::newFrameBuffer(const FrameBuffer::Descriptor& descriptor) const
{
    return utils::makeShared<MetalFrameBuffer>(descriptor).staticCast<FrameBuffer>();
}

#ifdef GFX_BUILD_IMGUI
void MetalGraphicAPI::initImgui(ImGuiConfigFlags flags)
{
    ImGui::CreateContext();
    
    ImGui::GetIO().ConfigFlags = flags;

    m_window->imGuiInit();
    ImGui_ImplMetal_Init(m_mtlDevice);
}
#endif

void MetalGraphicAPI::beginFrame() { @autoreleasepool
{
    m_commandBuffer = [[m_commandQueue commandBuffer] retain];
    assert(m_commandBuffer); // TODO use throw
}}

void MetalGraphicAPI::beginRenderPass() { @autoreleasepool
{
    if (m_window->currentDrawable() == nullptr)
        m_window->makeCurrentDrawables();

    MTLRenderPassDescriptor* renderPassDescriptor = [[[MTLRenderPassDescriptor alloc] init] autorelease];
    
    renderPassDescriptor.colorAttachments[0].loadAction = m_nextPassLoadAction == LoadAction::clear ? MTLLoadActionClear : MTLLoadActionLoad;
    renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(m_nextPassClearColor.r, m_nextPassClearColor.g, m_nextPassClearColor.b, m_nextPassClearColor.a);
    renderPassDescriptor.colorAttachments[0].texture = m_window->currentDrawable().texture;

    renderPassDescriptor.depthAttachment.loadAction = m_nextPassLoadAction == LoadAction::clear ? MTLLoadActionClear : MTLLoadActionLoad;
    renderPassDescriptor.depthAttachment.clearDepth = 1.0;
    renderPassDescriptor.depthAttachment.storeAction = MTLStoreActionStore;
    renderPassDescriptor.depthAttachment.texture = m_window->currentDepthTexture().mtlTexture();

    m_commandEncoder = [[m_commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor] retain];
    if (m_commandEncoder == nil)
        throw RenderCommandEncoderCreationError();
}}

void MetalGraphicAPI::beginRenderPass(const utils::SharedPtr<FrameBuffer>& target) { @autoreleasepool
{
    m_frameBuffer = target.forceDynamicCast<MetalFrameBuffer>();
    MTLRenderPassDescriptor* renderPassDescriptor = [[[MTLRenderPassDescriptor alloc] init] autorelease];

    renderPassDescriptor.colorAttachments[0].loadAction = m_nextPassLoadAction == LoadAction::clear ? MTLLoadActionClear : MTLLoadActionLoad;
    renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(m_nextPassClearColor.r, m_nextPassClearColor.g, m_nextPassClearColor.b, m_nextPassClearColor.a);
    renderPassDescriptor.colorAttachments[0].texture = m_frameBuffer->mtlColorTexture()->mtlTexture();

    renderPassDescriptor.depthAttachment.loadAction = m_nextPassLoadAction == LoadAction::clear ? MTLLoadActionClear : MTLLoadActionLoad;
    renderPassDescriptor.depthAttachment.clearDepth = 1.0;
    renderPassDescriptor.depthAttachment.storeAction = MTLStoreActionStore;
    renderPassDescriptor.depthAttachment.texture = m_frameBuffer->mtlDepthTexture()->mtlTexture();

    m_commandEncoder = [[m_commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor] retain];
    if (m_commandEncoder == nil)
        throw RenderCommandEncoderCreationError();
}}

void MetalGraphicAPI::beginRenderPass(const utils::SharedPtr<RenderTarget>& rt)
{
    if (auto win = rt.dynamicCast<Window>())
        beginRenderPass();
    else if (auto fb = rt.dynamicCast<FrameBuffer>())
        beginRenderPass(fb);
    else
        UNREACHABLE;
}

#ifdef GFX_BUILD_IMGUI
void MetalGraphicAPI::beginImguiRenderPass() { @autoreleasepool
{
    beginRenderPass();

    MTLRenderPassDescriptor* renderPassDescriptor = [[[MTLRenderPassDescriptor alloc] init] autorelease];
    renderPassDescriptor.colorAttachments[0].texture = m_window->currentDrawable().texture;
    renderPassDescriptor.depthAttachment.texture = m_window->currentDepthTexture().mtlTexture();

    m_window->imGuiNewFrame();
    ImGui_ImplMetal_NewFrame(renderPassDescriptor);
    ImGui::NewFrame();

    m_isImguiRenderPass = true;
    m_isImguiFrame = true;
}}
#endif

void MetalGraphicAPI::useGraphicsPipeline(const utils::SharedPtr<GraphicPipeline>& pipeline) { @autoreleasepool
{
    m_graphicPipeline = pipeline.forceDynamicCast<MetalGraphicPipeline>();
    [m_commandEncoder setRenderPipelineState:m_graphicPipeline->renderPipelineState()];
    [m_commandEncoder setDepthStencilState:m_graphicPipeline->depthStencilState()];
}}

void MetalGraphicAPI::setVertexBuffer(const utils::SharedPtr<Buffer>& buffer, utils::uint64 idx) { @autoreleasepool
{
    utils::Dictionary<utils::uint64, utils::SharedPtr<MetalBuffer>>::Iterator it = m_vertexBuffers.find(idx);
    if (it == m_vertexBuffers.end())
        it = m_vertexBuffers.insert(idx, utils::SharedPtr<MetalBuffer>());
    it->val = buffer.forceDynamicCast<MetalBuffer>();
    [m_commandEncoder setVertexBuffer:it->val->mtlBuffer() offset:0 atIndex:idx];
}}

void MetalGraphicAPI::setFragmentBuffer(const utils::SharedPtr<Buffer>& buffer, utils::uint64 idx) { @autoreleasepool
{
    utils::Dictionary<utils::uint64, utils::SharedPtr<MetalBuffer>>::Iterator it = m_fragmentBuffers.find(idx);
    if (it == m_fragmentBuffers.end())
        it = m_fragmentBuffers.insert(idx, utils::SharedPtr<MetalBuffer>());
    it->val = buffer.forceDynamicCast<MetalBuffer>();
    [m_commandEncoder setFragmentBuffer:it->val->mtlBuffer() offset:0 atIndex:idx];
}}

void MetalGraphicAPI::setFragmentTexture(const utils::SharedPtr<Texture>& texture, utils::uint64 idx) { @autoreleasepool
{
    utils::Dictionary<utils::uint64, utils::SharedPtr<MetalTexture>>::Iterator it = m_fragmentTextures.find(idx);
    if (it == m_fragmentTextures.end())
        it = m_fragmentTextures.insert(idx, utils::SharedPtr<MetalTexture>());
    it->val = texture.forceDynamicCast<MetalTexture>();
    [m_commandEncoder setFragmentTexture:it->val->mtlTexture() atIndex:idx];
}}

void MetalGraphicAPI::setFragmentTexture(const utils::SharedPtr<Texture>& texture, utils::uint64 textureIdx, const utils::SharedPtr<Sampler>& sampler, utils::uint64 samplerIdx) { @autoreleasepool
{
    utils::Dictionary<utils::uint64, utils::SharedPtr<MetalTexture>>::Iterator textureIt = m_fragmentTextures.find(textureIdx);
    if (textureIt == m_fragmentTextures.end())
        textureIt = m_fragmentTextures.insert(textureIdx, utils::SharedPtr<MetalTexture>());
    textureIt->val = texture.forceDynamicCast<MetalTexture>();

    utils::Dictionary<utils::uint64, utils::SharedPtr<MetalSampler>>::Iterator samplerIt = m_fragmentSamplers.find(samplerIdx);
    if (samplerIt == m_fragmentSamplers.end())
        samplerIt = m_fragmentSamplers.insert(samplerIdx, utils::SharedPtr<MetalSampler>());
    samplerIt->val = sampler.forceDynamicCast<MetalSampler>();

    [m_commandEncoder setFragmentTexture:textureIt->val->mtlTexture() atIndex:textureIdx];
    [m_commandEncoder setFragmentSamplerState:samplerIt->val->mtlSamplerState() atIndex:samplerIdx];
}}

void MetalGraphicAPI::drawVertices(utils::uint32 start, utils::uint32 count) { @autoreleasepool
{
    [m_commandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:start vertexCount:count];
}}

void MetalGraphicAPI::drawIndexedVertices(const utils::SharedPtr<Buffer>& buffer) { @autoreleasepool
{
    m_indexBuffer = buffer.forceDynamicCast<MetalBuffer>();
    [m_commandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                 indexCount:m_indexBuffer->size() / sizeof(utils::uint32)
                                  indexType:MTLIndexTypeUInt32
                                indexBuffer:m_indexBuffer->mtlBuffer()
                          indexBufferOffset:0];
}}

void MetalGraphicAPI::endRenderPass()
{
    #ifdef GFX_BUILD_IMGUI
    if (m_isImguiRenderPass)
    {
        ImGui::Render();
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), m_commandBuffer, m_commandEncoder);
        m_isImguiRenderPass = false;
    }
    #endif

    [m_commandEncoder endEncoding];
    [m_commandEncoder release];

    m_indexBuffer.clear();
    m_fragmentSamplers.clear();
    m_fragmentTextures.clear();
    m_fragmentBuffers.clear();
    m_vertexBuffers.clear();
    m_graphicPipeline.clear();
    m_frameBuffer.clear();
}

void MetalGraphicAPI::endFrame()
{
    #ifdef GFX_BUILD_IMGUI
    if (m_isImguiFrame && (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        m_isImguiFrame = false;
    }
    #endif
    
    if (m_window->currentDrawable() != nullptr)
    {
        [m_commandBuffer presentDrawable:m_window->currentDrawable()];
        m_window->clearCurrentDrawables();
    }
    [m_commandBuffer commit];
    [m_commandBuffer waitUntilCompleted];
    [m_commandBuffer release];
}

#ifdef GFX_BUILD_IMGUI
void MetalGraphicAPI::terminateImGui()
{
    ImGui_ImplMetal_Shutdown();
    m_window->imGuiShutdown();
    ImGui::DestroyContext();
}
#endif

MetalGraphicAPI::~MetalGraphicAPI() { @autoreleasepool
{
    [m_commandQueue release];
    [m_mtlDevice release];
}}

}
