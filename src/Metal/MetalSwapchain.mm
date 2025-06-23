/*
 * ---------------------------------------------------
 * MetalSwapchain.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/01 19:24:30
 * ---------------------------------------------------
 */

#include "Graphics/Swapchain.hpp"
#include "Graphics/Drawable.hpp"

#include "Metal/MetalSwapchain.hpp"
#include "Metal/MetalDevice.hpp"
#include "Metal/MetalDrawable.hpp"
#include "Metal/MetalEnums.hpp"
#include "Metal/MetalSurface.hpp"

#import <QuartzCore/CAMetalLayer.h>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <cassert>
    #include <memory>
    namespace ext = std;
#endif

namespace gfx
{

MetalSwapchain::MetalSwapchain(const MetalDevice& mtlDevice, const Swapchain::Descriptor& desc) { @autoreleasepool
{
    assert(desc.surface);
    MetalSurface* mtlSurface = static_cast<MetalSurface*>(desc.surface);

    m_mtlLayer = [mtlSurface->mtlLayer() retain];
    m_mtlLayer.device = mtlDevice.mtlDevice();
    m_mtlLayer.drawableSize = CGSize{CGFloat(desc.width), CGFloat(desc.height)};
    m_mtlLayer.pixelFormat = toMTLPixelFormat(desc.pixelFormat);
}}
    
ext::shared_ptr<Drawable> MetalSwapchain::nextDrawable(void) { @autoreleasepool
{
    return ext::make_shared<MetalDrawable>([m_mtlLayer nextDrawable]);
}}

MetalSwapchain::~MetalSwapchain() { @autoreleasepool
{
    [m_mtlLayer release];
}}

}
