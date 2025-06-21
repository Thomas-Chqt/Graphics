/*
 * ---------------------------------------------------
 * Framebuffer.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/06 06:41:27
 * ---------------------------------------------------
 */

#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include "Graphics/RenderPass.hpp"
#include "Graphics/Texture.hpp"

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <cstdint>
    #include <vector>
    #include <memory>
    #include <cstddef>
    namespace ext = std;
#endif

namespace gfx
{

class Framebuffer
{
public:
    struct Descriptor
    {
        uint32_t width, height;
        RenderPass* renderPass;
        ext::vector<ext::shared_ptr<Texture>> colorAttachments;
        ext::shared_ptr<Texture> depthAttachment;
    };

public:
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&&)      = delete;

    virtual ext::vector<ext::shared_ptr<Texture>> colorAttachments(void) = 0;
    virtual const ext::vector<ext::shared_ptr<Texture>> colorAttachments(void) const = 0;

    virtual ext::shared_ptr<Texture> colorAttachment(size_t) = 0;
    virtual const ext::shared_ptr<Texture> colorAttachment(size_t) const = 0;

    virtual ext::shared_ptr<Texture> depthAttachment(void) = 0;
    virtual const ext::shared_ptr<Texture> depthAttachment(void) const = 0;

    virtual ~Framebuffer() = default;

protected:
    Framebuffer() = default;
    
public:
    Framebuffer& operator = (const Framebuffer&) = delete;
    Framebuffer& operator = (Framebuffer&&)      = delete;
};

}

#endif // FRAMEBUFFER_HPP
