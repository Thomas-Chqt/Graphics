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
#include "Graphics/Texture.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/SharedPtr.hpp"

namespace gfx
{

MetalFrameBuffer::MetalFrameBuffer(const utils::SharedPtr<Texture>& colorTexture)
{
    if (colorTexture)
        setColorTexture(colorTexture);
}

void MetalFrameBuffer::setColorTexture(const utils::SharedPtr<Texture>& texture)
{
    if (utils::SharedPtr<MetalTexture> mtlTexture = texture.dynamicCast<MetalTexture>())
        m_colorTexture = mtlTexture;
    else
        throw utils::RuntimeError("Texture is not MetalTexture");
}

}