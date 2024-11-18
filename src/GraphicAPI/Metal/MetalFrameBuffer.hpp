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

    MetalFrameBuffer(const FrameBuffer::Descriptor&);

    inline utils::uint32 width() override { return colorTexture()->width(); }
    inline utils::uint32 height() override { return colorTexture()->height(); }

    void setColorTexture(const utils::SharedPtr<Texture>&) override;
    inline utils::SharedPtr<Texture> colorTexture() override { return m_colorTexture.staticCast<Texture>(); };

    void setDepthTexture(const utils::SharedPtr<Texture>&) override;
    inline utils::SharedPtr<Texture> depthTexture() override { return m_depthTexture.staticCast<Texture>(); };

    inline utils::SharedPtr<MetalTexture> mtlColorTexture() { return m_colorTexture; }
    inline utils::SharedPtr<MetalTexture> mtlDepthTexture() { return m_depthTexture; }

    ~MetalFrameBuffer() override = default;

private:
    utils::SharedPtr<MetalTexture> m_colorTexture;
    utils::SharedPtr<MetalTexture> m_depthTexture;

public:
    MetalFrameBuffer& operator = (const MetalFrameBuffer&) = delete;
    MetalFrameBuffer& operator = (MetalFrameBuffer&&)      = delete;
};

}

#endif // METALFRAMEBUFFER_HPP