/*
 * ---------------------------------------------------
 * MetalCommandBuffer.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/08 10:37:29
 * ---------------------------------------------------
 */

#include "Graphics/RenderPass.hpp"
#include "Graphics/Framebuffer.hpp"

#include "Metal/MetalCommandBuffer.hpp"
#include "Metal/MetalRenderPass.hpp"
#include "Metal/MetalFramebuffer.hpp"
#include "Metal/MetalSwapchain.hpp"
#include "Metal/MetalTexture.hpp"

#import <Metal/Metal.h>
#include <memory>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <cassert>
    #include <utility>
    namespace ext = std;
#endif

namespace gfx
{

MetalCommandBuffer::MetalCommandBuffer(MetalCommandBuffer&& other)
    : m_mtlCommandBuffer(other.m_mtlCommandBuffer), m_commandEncoder(other.m_commandEncoder)
{
    other.m_mtlCommandBuffer = nil;
    other.m_commandEncoder = nil;
}

MetalCommandBuffer::MetalCommandBuffer(id<MTLCommandQueue> queue) { @autoreleasepool
{
    m_mtlCommandBuffer = [[queue commandBuffer] retain];
}}

void MetalCommandBuffer::beginRenderPass(const ext::shared_ptr<RenderPass>& _renderPass, const ext::shared_ptr<Framebuffer>& _frameBuffer) { @autoreleasepool
{
    const ext::shared_ptr<MetalRenderPass>& renderPass = ext::dynamic_pointer_cast<MetalRenderPass>(_renderPass);
    const ext::shared_ptr<MetalFramebuffer> framebuffer = ext::dynamic_pointer_cast<MetalFramebuffer>(_frameBuffer);

    MTLRenderPassDescriptor* mtlRenderPassDescriptor = [[renderPass->mtlRenderPassDescriptor() copy] autorelease];
    for (int i = 0; auto& colorAttachment : framebuffer->colorAttachments())
        mtlRenderPassDescriptor.colorAttachments[i].texture = dynamic_cast<MetalTexture*>(colorAttachment.get())->mtltexture();
    if (const MetalTexture* depthAttachment = dynamic_cast<MetalTexture*>(framebuffer->depthAttachment().get()))
        mtlRenderPassDescriptor.depthAttachment.texture = depthAttachment->mtltexture();

    m_commandEncoder = [[m_mtlCommandBuffer renderCommandEncoderWithDescriptor: mtlRenderPassDescriptor] retain];

    m_usedFramebuffers.push_back(framebuffer);
}}

void MetalCommandBuffer::endRenderPass(void) { @autoreleasepool
{
    assert(m_commandEncoder);
    [m_commandEncoder endEncoding];
    [m_commandEncoder release];
    m_commandEncoder = nil;
}}

#define CLEAR                       \
    if (m_commandEncoder)           \
        [m_commandEncoder release]; \
    if (m_mtlCommandBuffer)         \
        [m_mtlCommandBuffer release]

MetalCommandBuffer::~MetalCommandBuffer() { @autoreleasepool
{
    CLEAR;
}}

MetalCommandBuffer& MetalCommandBuffer::operator = (MetalCommandBuffer&& other) { @autoreleasepool
{
    if (this != &other)
    {
        CLEAR;

        (void)CommandBuffer::operator=(std::move(other));

        m_commandEncoder = ext::exchange(other.m_commandEncoder, nil);
        m_mtlCommandBuffer = ext::exchange(other.m_mtlCommandBuffer, nil);
    }
    return *this;
}}

}
