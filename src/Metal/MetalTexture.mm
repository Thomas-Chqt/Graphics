/*
 * ---------------------------------------------------
 * MetalTexture.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/06 18:20:36
 * ---------------------------------------------------
 */

#include "Graphics/Texture.hpp"
#include "Graphics/Enums.hpp"
#include "Metal/MetalEnums.hpp"

#include "Metal/MetalTexture.hpp"

namespace gfx
{

MetalTexture::MetalTexture(id<MTLTexture> mtltexture)
    : m_shouldDestroyTexture(false), m_mtlTexture(mtltexture)
{
}

uint32_t MetalTexture::width() const { @autoreleasepool
{
    return m_mtlTexture.width;
}}

uint32_t MetalTexture::height() const { @autoreleasepool
{
    return m_mtlTexture.height;
}}

PixelFormat MetalTexture::pixelFormat(void) const { @autoreleasepool
{
    return toPixelFormat([m_mtlTexture pixelFormat]);   
}}

MetalTexture::~MetalTexture()
{
    if (m_shouldDestroyTexture)
        [m_mtlTexture release];
}

}
