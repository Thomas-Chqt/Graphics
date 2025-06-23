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

MetalTexture::MetalTexture(const id<MTLTexture>& mtltexture) { @autoreleasepool
{
    m_mtlTexture = [mtltexture retain];
}}

uint32_t MetalTexture::width() const { @autoreleasepool
{
    return static_cast<uint32_t>(m_mtlTexture.width);
}}

uint32_t MetalTexture::height() const { @autoreleasepool
{
    return static_cast<uint32_t>(m_mtlTexture.height);
}}

PixelFormat MetalTexture::pixelFormat(void) const { @autoreleasepool
{
    return toPixelFormat([m_mtlTexture pixelFormat]);   
}}

MetalTexture::~MetalTexture() { @autoreleasepool
{
    [m_mtlTexture release];
}}

}
