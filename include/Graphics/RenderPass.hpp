/*
 * ---------------------------------------------------
 * RenderPass.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/03 17:51:15
 * ---------------------------------------------------
 */

#ifndef RENDERPASS_HPP
#define RENDERPASS_HPP

#include "Graphics/Enums.hpp"

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <optional>
    #include <vector>
    namespace ext = std;
#endif

namespace gfx
{

struct AttachmentDescriptor
{
    PixelFormat format;
    LoadAction loadAction = LoadAction::load;
    union { float clearColor[4]; float clearDepth; };
};

class RenderPass
{
public:
    struct Descriptor
    {
        ext::vector<AttachmentDescriptor> colorAttachments;
        ext::optional<AttachmentDescriptor> depthAttachment;
    };

public:
    RenderPass(const RenderPass&) = delete;
    RenderPass(RenderPass&&)      = delete;

    virtual ~RenderPass() = default;

protected:
    RenderPass() = default;
    
public:
    RenderPass& operator = (const RenderPass&) = delete;
    RenderPass& operator = (RenderPass&&)      = delete;
};

}

#endif // RENDERPASS_HPP
