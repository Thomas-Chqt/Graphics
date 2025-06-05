/*
 * ---------------------------------------------------
 * MetalSwapchain.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/01 19:24:30
 * ---------------------------------------------------
 */

#include "Metal/MetalSwapchain.hpp"
#include "Metal/MetalDevice.hpp"
#include "Metal/MetalEnums.hpp"
#include "Metal/MetalSurface.hpp"

#import <QuartzCore/CAMetalLayer.h>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <cassert>
    namespace ext = std;
#endif

namespace gfx
{

MetalSwapchain::MetalSwapchain(const MetalDevice& mtlDevice, const Swapchain::Descriptor& desc)
{
    assert(desc.surface);
    MetalSurface* mtlSurface = static_cast<MetalSurface*>(desc.surface);

    m_mtlLayer = [mtlSurface->mtlLayer() retain];
    m_mtlLayer.device = mtlDevice.mtlDevice();
    m_mtlLayer.drawableSize = CGSize{CGFloat(desc.width), CGFloat(desc.height)};
    m_mtlLayer.pixelFormat = toMTLPixelFormat(desc.pixelFormat);
}

MetalSwapchain::~MetalSwapchain()
{
    [m_mtlLayer release];
}

}
