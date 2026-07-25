#include "Metal/MetalPassDescriptor.hpp"

#include "Metal/MetalEnums.hpp"
#include "Metal/MetalTexture.hpp"

#include <cassert>
#include <type_traits>
#include <utility>

namespace gfx
{

namespace
{
MTLClearColor toMTLClearColor(const ClearValue& clearValue)
{
    return std::visit([]<typename T>(const T& clear) -> MTLClearColor {
        if constexpr (std::is_same_v<T, ClearFloatColor> || std::is_same_v<T, ClearUIntColor>)
            return MTLClearColorMake(clear.value[0], clear.value[1], clear.value[2], clear.value[3]);
        else
            std::unreachable();
    }, clearValue.value);
}

double toMTLClearDepth(const ClearValue& clearValue)
{
    const auto* clearDepth = std::get_if<ClearDepth>(&clearValue.value);
    assert(clearDepth);
    return clearDepth->value;
}
} // namespace

MetalRenderPassDescriptor::MetalRenderPassDescriptor()
    : m_mtlRenderPassDescriptor([[MTLRenderPassDescriptor alloc] init])
{
}

void MetalRenderPassDescriptor::setColorAttachments(std::vector<Attachment> attachments)
{
    assert(attachments.size() <= 8);

    for (size_t i = 0; i < m_colorAttachments.size(); ++i)
        m_mtlRenderPassDescriptor.colorAttachments[i].texture = nil;

    m_colorAttachments = std::move(attachments);
    for (size_t i = 0; i < m_colorAttachments.size(); ++i)
    {
        const auto& attachment = m_colorAttachments[i];
        auto texture = std::dynamic_pointer_cast<MetalTexture>(attachment.texture);
        assert(texture);

        auto* mtlAttachment = m_mtlRenderPassDescriptor.colorAttachments[i];
        mtlAttachment.loadAction = toMTLLoadAction(attachment.loadAction);
        mtlAttachment.storeAction = MTLStoreActionStore;
        mtlAttachment.clearColor = toMTLClearColor(attachment.clearValue);
        mtlAttachment.texture = texture->mtltexture();
    }
}

void MetalRenderPassDescriptor::setDepthAttachment(std::optional<Attachment> attachment)
{
    m_depthAttachment = std::move(attachment);
    if (m_depthAttachment.has_value() == false)
    {
        m_mtlRenderPassDescriptor.depthAttachment.texture = nil;
        return;
    }

    auto texture = std::dynamic_pointer_cast<MetalTexture>(m_depthAttachment->texture);
    assert(texture);

    m_mtlRenderPassDescriptor.depthAttachment.loadAction = toMTLLoadAction(m_depthAttachment->loadAction);
    m_mtlRenderPassDescriptor.depthAttachment.storeAction = MTLStoreActionStore;
    m_mtlRenderPassDescriptor.depthAttachment.clearDepth = toMTLClearDepth(m_depthAttachment->clearValue);
    m_mtlRenderPassDescriptor.depthAttachment.texture = texture->mtltexture();
}

MetalBlitPassDescriptor::MetalBlitPassDescriptor()
    : m_mtlBlitPassDescriptor([[MTLBlitPassDescriptor alloc] init])
{
}

MetalComputePassDescriptor::MetalComputePassDescriptor()
    : m_mtlComputePassDescriptor([[MTLComputePassDescriptor alloc] init])
{
}

} // namespace gfx
