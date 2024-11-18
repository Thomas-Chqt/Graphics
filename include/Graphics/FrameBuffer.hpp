/*
 * ---------------------------------------------------
 * FrameBuffer.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/10 13:51:44
 * ---------------------------------------------------
 */

#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include "Graphics/RenderTarget.hpp"
#include "Texture.hpp"
#include "UtilsCPP/SharedPtr.hpp"

namespace gfx
{

class FrameBuffer : public RenderTarget
{
public:
    struct Descriptor
    {
        utils::SharedPtr<Texture> colorTexture;
        utils::SharedPtr<Texture> depthTexture;
    };

public:
    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer(FrameBuffer&&)      = delete;

    virtual utils::uint32 width() override = 0;
    virtual utils::uint32 height() override = 0;

    virtual void setColorTexture(const utils::SharedPtr<Texture>&) = 0;
    virtual utils::SharedPtr<Texture> colorTexture() = 0;

    virtual void setDepthTexture(const utils::SharedPtr<Texture>&) = 0;
    virtual utils::SharedPtr<Texture> depthTexture() = 0;

    virtual ~FrameBuffer() = default;

protected:
    FrameBuffer() = default;

public:
    FrameBuffer& operator = (const FrameBuffer&) = delete;
    FrameBuffer& operator = (FrameBuffer&&)      = delete;
};

} // namespace gfx

#endif // FRAMEBUFFER_HPP
