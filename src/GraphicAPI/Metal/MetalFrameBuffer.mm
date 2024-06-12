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
#include <algorithm>

namespace gfx
{

MetalLayerFrameBuffer::MetalLayerFrameBuffer(CAMetalLayer* metalLayer) : m_metalLayer(metalLayer)
{
}

id<MTLTexture> MetalLayerFrameBuffer::mtlRenderTexture()
{
    if (m_currentDrawable == nullptr)
        m_currentDrawable = [[m_metalLayer nextDrawable] retain];
    return m_currentDrawable.texture;
}

void MetalLayerFrameBuffer::releaseCurrentDrawable()
{
    [m_currentDrawable release];
    m_currentDrawable = nullptr;
}

MetalLayerFrameBuffer::~MetalLayerFrameBuffer()
{
    if (m_currentDrawable)
        [m_currentDrawable release];
}

MetalTextureFrameBuffer::MetalTextureFrameBuffer(MetalTexture&& colorTexture) : m_colorTexture(std::move(colorTexture))
{
}

id<MTLTexture> MetalTextureFrameBuffer::mtlRenderTexture()
{
    return m_colorTexture.mtlTexture();
}

}