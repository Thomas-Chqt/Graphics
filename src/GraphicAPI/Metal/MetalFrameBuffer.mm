/*
 * ---------------------------------------------------
 * MetalFrameBuffer.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/10 13:59:51
 * ---------------------------------------------------
 */

#include "GraphicAPI/Metal/MetalFrameBuffer.hpp"
#include "GraphicAPI/Metal/MetalTexture.hpp"

namespace gfx
{

MetalTextureFrameBuffer::MetalTextureFrameBuffer(MetalTexture&& colorTexture) : m_colorTexture(std::move(colorTexture))
{
}

MetalDrawableFrameBuffer::MetalDrawableFrameBuffer(id<CAMetalDrawable> caDrawable)
{

}

}