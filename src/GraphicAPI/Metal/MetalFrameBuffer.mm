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
#include "Graphics/FrameBuffer.hpp"
#include "Graphics/Texture.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/SharedPtr.hpp"

namespace gfx
{

MetalFrameBuffer::MetalFrameBuffer(const FrameBuffer::Descriptor& descriptor)
{
    if (descriptor.colorTexture)
        MetalFrameBuffer::setColorTexture(descriptor.colorTexture);
    if (descriptor.depthTexture)
        MetalFrameBuffer::setDepthTexture(descriptor.depthTexture);
}

void MetalFrameBuffer::setColorTexture(const utils::SharedPtr<Texture>& texture)
{
    m_colorTexture = texture.forceDynamicCast<MetalTexture>();
}

void MetalFrameBuffer::setDepthTexture(const utils::SharedPtr<Texture>& texture)
{
    m_depthTexture = texture.forceDynamicCast<MetalTexture>();
}

}