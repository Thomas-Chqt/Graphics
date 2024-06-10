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

#include "GraphicAPI/Metal/MetalGraphicAPI.hpp"
#include "GraphicAPI/Metal/MetalTexture.hpp"
#include "Graphics/FrameBuffer.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    template<typename T> using id = void*;
    class MTLDevice;
#endif // OBJCPP

namespace gfx
{

class MetalFrameBuffer : public FrameBuffer
{
private:
    friend utils::SharedPtr<FrameBuffer> MetalGraphicAPI::newFrameBuffer(utils::uint32, utils::uint32) const;

public:
    MetalFrameBuffer()                        = delete;
    MetalFrameBuffer(const MetalFrameBuffer&) = delete;
    MetalFrameBuffer(MetalFrameBuffer&&)      = delete;

    inline const MetalTexture& colorTexture() const { return m_colorTexture; }
    
    ~MetalFrameBuffer() override = default;

private:
    MetalFrameBuffer(MetalTexture&& colorTexture);

    MetalTexture m_colorTexture;

public:
    MetalFrameBuffer& operator = (const MetalFrameBuffer&) = delete;
    MetalFrameBuffer& operator = (MetalFrameBuffer&&)      = delete;
};

}

#endif // METALFRAMEBUFFER_HPP