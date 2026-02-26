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

#import "Metal/MetalEnums.hpp"
#include <memory>

namespace gfx
{

MetalSwapchain::MetalSwapchain(const MetalDevice& device, const Swapchain::Descriptor& desc)
    : m_swapchainImagesDescriptor{
        .type = TextureType::texture2d,
        .width = desc.width,
        .height = desc.height,
        .pixelFormat = desc.pixelFormat,
        .usages = TextureUsage::colorAttachment,
        .storageMode = ResourceStorageMode::deviceLocal
    }
{
    @autoreleasepool {
        assert(desc.surface);
        auto* mtlSurface = dynamic_cast<MetalSurface*>(desc.surface);
        assert(mtlSurface);

        m_mtlLayer = mtlSurface->mtlLayer();
        m_mtlLayer.device = device.mtlDevice();
        m_mtlLayer.drawableSize = CGSize{CGFloat(desc.width), CGFloat(desc.height)};
        m_mtlLayer.pixelFormat = toMTLPixelFormat(desc.pixelFormat);

        m_drawables.resize(desc.drawableCount);
        for (auto& drawable : m_drawables)
            drawable = std::make_shared<MetalDrawable>(m_swapchainImagesDescriptor);

    }
}

std::shared_ptr<Drawable> MetalSwapchain::nextDrawable() { @autoreleasepool
{
    ZoneScoped;
    std::shared_ptr<MetalDrawable> nextDrawable = m_drawables.at(m_nextDrawableIndex);
    m_nextDrawableIndex = (m_nextDrawableIndex + 1) % m_drawables.size();
    nextDrawable->setMtlDrawable([m_mtlLayer nextDrawable]);
    return nextDrawable;
}}

}
