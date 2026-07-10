#ifndef METALPASSDESCRIPTOR_HPP
#define METALPASSDESCRIPTOR_HPP

#include "Graphics/BlitPassDescriptor.hpp"
#include "Graphics/RenderPassDescriptor.hpp"

#if !defined(__OBJC__)
#error this file can only be used in objective c
#endif

namespace gfx
{

class MetalRenderPassDescriptor final : public RenderPassDescriptor
{
public:
    MetalRenderPassDescriptor();

    const std::vector<Attachment>& colorAttachments() const override { return m_colorAttachments; }
    void setColorAttachments(std::vector<Attachment>) override;

    const std::optional<Attachment>& depthAttachment() const override { return m_depthAttachment; }
    void setDepthAttachment(std::optional<Attachment>) override;

    MTLRenderPassDescriptor* mtlRenderPassDescriptor() const { return m_mtlRenderPassDescriptor; }

private:
    std::vector<Attachment> m_colorAttachments;
    std::optional<Attachment> m_depthAttachment;
    MTLRenderPassDescriptor* m_mtlRenderPassDescriptor = nil;
};

class MetalBlitPassDescriptor final : public BlitPassDescriptor
{
public:
    MetalBlitPassDescriptor();

    MTLBlitPassDescriptor* mtlBlitPassDescriptor() const { return m_mtlBlitPassDescriptor; }

private:
    MTLBlitPassDescriptor* m_mtlBlitPassDescriptor = nil;
};

} // namespace gfx

#endif // METALPASSDESCRIPTOR_HPP
