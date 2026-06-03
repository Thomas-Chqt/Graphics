/*
 * ---------------------------------------------------
 * MetalSurface.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/30 07:47:06
 * ---------------------------------------------------
 */

#include "Graphics/Enums.hpp"

#include "Metal/MetalSurface.hpp"

namespace gfx
{

MetalSurface::MetalSurface(CAMetalLayer* layer)
    : m_mtlLayer(layer)
{
}

const std::set<PixelFormat> MetalSurface::supportedPixelFormats(const Device&) const
{
    return {
        PixelFormat::BGRA8Unorm,
        PixelFormat::BGRA8Unorm_sRGB,
    };
}

const std::set<PresentMode> MetalSurface::supportedPresentModes(const Device&) const
{
    return {
        PresentMode::fifo
    };
}

}
