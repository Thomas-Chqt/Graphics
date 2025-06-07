/*
 * ---------------------------------------------------
 * MetalFramebuffer.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/06 17:10:09
 * ---------------------------------------------------
 */

#include "Graphics/Framebuffer.hpp"

#include "Metal/MetalFramebuffer.hpp"
#include "Metal/MetalTexture.hpp"
#include <algorithm>
#include <utility>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    #include <cassert>
    namespace ext = std;
#endif

namespace gfx
{

MetalFramebuffer::MetalFramebuffer(const Framebuffer::Descriptor& desc)
{
    m_colorAttachments.resize(desc.colorAttachments.size());
    uint32_t i = 0;
    for (auto& attachment : desc.colorAttachments) {
        m_colorAttachments[i] = ext::dynamic_pointer_cast<MetalTexture>(attachment);
        assert(m_colorAttachments[i]);
        i++;
    }
    if (desc.depthAttachment) {
        m_depthAttachment = ext::dynamic_pointer_cast<MetalTexture>(desc.depthAttachment);
        assert(m_depthAttachment);
    }
}

MetalFramebuffer::MetalFramebuffer(const ext::vector<ext::shared_ptr<MetalTexture>>& colorAttachments, const ext::shared_ptr<MetalTexture>& depthAttachment)
    : m_colorAttachments(colorAttachments), m_depthAttachment(depthAttachment)
{
}

MetalFramebuffer& MetalFramebuffer::operator = (MetalFramebuffer&& other)
{
    if (&other != this)
    {
        m_colorAttachments = std::move(other.m_colorAttachments);
        m_depthAttachment = ext::move(other.m_depthAttachment);
    }
    return *this;
}

}
