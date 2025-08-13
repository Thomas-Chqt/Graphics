/*
 * ---------------------------------------------------
 * MetalDrawable.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/23 13:03:22
 * ---------------------------------------------------
 */

#include "Metal/MetalDrawable.hpp"
#include "Metal/MetalTexture.hpp"

#import <QuartzCore/CAMetalLayer.h>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    namespace ext = std;
#endif

namespace gfx
{

MetalDrawable::MetalDrawable(const MetalDevice* device, id<CAMetalDrawable> mtlDrawable)
    : m_device(device) { @autoreleasepool
{
    m_mtlDrawable = [mtlDrawable retain];
}}

ext::shared_ptr<Texture> MetalDrawable::texture() const { @autoreleasepool
{
    return ext::make_shared<MetalTexture>(m_device, m_mtlDrawable.texture);
}}

MetalDrawable::~MetalDrawable()
{
    [m_mtlDrawable release];
}

}
