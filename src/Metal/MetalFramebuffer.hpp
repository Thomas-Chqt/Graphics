/*
 * ---------------------------------------------------
 * MetalFramebuffer.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/06 16:45:52
 * ---------------------------------------------------
 */

#ifndef METALFRAMEBUFFER_HPP
#define METALFRAMEBUFFER_HPP

#include "Graphics/Framebuffer.hpp"

#include "Metal/MetalTexture.hpp"

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    #include <memory>
    namespace ext = std;
#endif

namespace gfx
{

class MetalFramebuffer : public Framebuffer
{
public:
    MetalFramebuffer() = default;
    MetalFramebuffer(const MetalFramebuffer&) = delete;
    MetalFramebuffer(MetalFramebuffer&&)      = delete;

    MetalFramebuffer(const Framebuffer::Descriptor&);
    MetalFramebuffer(const ext::vector<ext::shared_ptr<MetalTexture>>& colorAttachments, const ext::shared_ptr<MetalTexture>& depthAttachment);

    ext::vector<ext::shared_ptr<Texture>> colorAttachments(void) override;
    const ext::vector<ext::shared_ptr<Texture>> colorAttachments(void) const override;

    inline ext::shared_ptr<Texture> colorAttachment(size_t i) override { return m_colorAttachments[i]; };
    inline const ext::shared_ptr<Texture> colorAttachment(size_t i) const override { return m_colorAttachments[i]; };

    inline ext::shared_ptr<Texture> depthAttachment(void) override { return m_depthAttachment; };
    inline const ext::shared_ptr<Texture> depthAttachment(void) const override { return m_depthAttachment; };

    ~MetalFramebuffer() = default;

private:
    ext::vector<ext::shared_ptr<MetalTexture>> m_colorAttachments;
    ext::shared_ptr<MetalTexture> m_depthAttachment;
    
public:
    MetalFramebuffer& operator = (const MetalFramebuffer&) = delete;
    MetalFramebuffer& operator = (MetalFramebuffer&&);
};

}

#endif // METALFRAMEBUFFER_HPP
