/*
 * ---------------------------------------------------
 * MetalSwapchain.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/01 19:24:30
 * ---------------------------------------------------
 */

#include "Graphics/Framebuffer.hpp"
#include "Graphics/Swapchain.hpp"

#include "Metal/MetalSwapchain.hpp"
#include "Metal/MetalDevice.hpp"
#include "Metal/MetalEnums.hpp"
#include "Metal/MetalFramebuffer.hpp"
#include "Metal/MetalSurface.hpp"
#include "Metal/MetalTexture.hpp"

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
    
ext::shared_ptr<Framebuffer> MetalSwapchain::nextFrameBuffer(void) { @autoreleasepool
{
    if (m_currentDrawable)
        [m_currentDrawable release];
    m_currentDrawable = [[m_mtlLayer nextDrawable] retain];
    
    ext::shared_ptr<MetalFramebuffer> framebuffer(new MetalFramebuffer(
        { ext::make_shared<MetalTexture>(m_currentDrawable.texture) },
        m_depthAttachments.empty() ? nullptr : m_depthAttachments[m_drawableIndex]
    ));

    m_drawableIndex = (m_drawableIndex + 1) % 3;

    return framebuffer;
}}

MetalSwapchain::~MetalSwapchain() { @autoreleasepool
{
    [m_mtlLayer release];
}}

}
