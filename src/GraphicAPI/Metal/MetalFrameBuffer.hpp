/*
 * ---------------------------------------------------
 * MetalFrameBuffer.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/10 13:55:02
 * ---------------------------------------------------
 */

#ifndef METALFRAMEBUFFER_HPP
# define METALFRAMEBUFFER_HPP

#include "GraphicAPI/Metal/MetalTexture.hpp"
#include "Graphics/FrameBuffer.hpp"
#include "Graphics/Texture.hpp"
#include "UtilsCPP/SharedPtr.hpp"

namespace gfx
{

class MetalFrameBuffer : public FrameBuffer
{
public:
    MetalFrameBuffer()                        = delete;
    MetalFrameBuffer(const MetalFrameBuffer&) = delete;
    MetalFrameBuffer(MetalFrameBuffer&&)      = delete;

    MetalFrameBuffer(const utils::SharedPtr<Texture>& colorTexture = utils::SharedPtr<Texture>());

    void setColorTexture(const utils::SharedPtr<Texture>&) override;
    inline utils::SharedPtr<Texture> colorTexture() override { return m_colorTexture.staticCast<Texture>(); };

    inline utils::SharedPtr<MetalTexture> mtlColorTexture() { return m_colorTexture; }

    virtual ~MetalFrameBuffer() override = default;

private:
    utils::SharedPtr<MetalTexture> m_colorTexture;

public:
    MetalFrameBuffer& operator = (const MetalFrameBuffer&) = delete;
    MetalFrameBuffer& operator = (MetalFrameBuffer&&)      = delete;
};

}

#endif // METALFRAMEBUFFER_HPP