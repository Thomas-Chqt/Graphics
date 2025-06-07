/*
 * ---------------------------------------------------
 * MetalFramebuffer.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/06 16:45:52
 * ---------------------------------------------------
 */

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

    ~MetalFramebuffer() = default;

private:
    ext::vector<ext::shared_ptr<MetalTexture>> m_colorAttachments;
    ext::shared_ptr<MetalTexture> m_depthAttachment;
    
public:
    MetalFramebuffer& operator = (const MetalFramebuffer&) = delete;
    MetalFramebuffer& operator = (MetalFramebuffer&&);
};

}
