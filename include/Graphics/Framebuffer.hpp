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

#include "Graphics/Texture.hpp"
#include "Graphics/Enums.hpp"

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    #include <memory>
    #include <optional>
    namespace ext = std;
#endif

namespace gfx
{


struct Framebuffer
{
    struct Attachment
    {
        LoadAction loadAction = LoadAction::load;
        union { float clearColor[4]; float clearDepth; };
        ext::shared_ptr<Texture> texture;
    };

    ext::vector<Attachment> colorAttachments;
    ext::optional<Attachment> depthAttachment;
};

}

#endif // FRAMEBUFFER_HPP
