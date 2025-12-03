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
#include "Metal/MetalSurface.hpp"

#import "Metal/MetalEnums.h"

namespace gfx
{

MetalSwapchain::MetalSwapchain(const MetalDevice& device, const Swapchain::Descriptor& desc) { @autoreleasepool
{
    assert(desc.surface);
    auto* mtlSurface = dynamic_cast<MetalSurface*>(desc.surface);
    assert(mtlSurface);

    m_mtlLayer = [mtlSurface->mtlLayer() retain];
    m_mtlLayer.device = device.mtlDevice();
    m_mtlLayer.drawableSize = CGSize{CGFloat(desc.width), CGFloat(desc.height)};
    m_mtlLayer.pixelFormat = toMTLPixelFormat(desc.pixelFormat);
}}

std::shared_ptr<Drawable> MetalSwapchain::nextDrawable() { @autoreleasepool
{
    ZoneScoped;
    return std::make_shared<MetalDrawable>([m_mtlLayer nextDrawable]);
}}

MetalSwapchain::~MetalSwapchain() { @autoreleasepool
{
    [m_mtlLayer release];
}}

}
