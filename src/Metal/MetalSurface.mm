/*
 * ---------------------------------------------------
 * MetalSurface.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/30 07:47:06
 * ---------------------------------------------------
 */

#include "Graphics/Enums.hpp"
#include "Graphics/Surface.hpp"
#include <array>

#include "Metal/MetalSurface.hpp"

namespace gfx
{

MetalSurface::MetalSurface(CAMetalLayer* layer)
    : m_mtlLayer(layer)
{
}

std::set<SurfaceFormat> MetalSurface::supportedSurfaceFormat(const Device&) const
{
    std::set<SurfaceFormat> surfaceFormats;
    for (PixelFormat pixelFormat : std::to_array({PixelFormat::BGRA8_unorm, PixelFormat::BGRA8_sRGB, PixelFormat::RGBA16_float})) {
        for (ColorSpace colorSpace : std::to_array({ColorSpace::sRGB_nonLinear, ColorSpace::displayP3_nonlinear})) {
            surfaceFormats.insert(SurfaceFormat{
                .pixelFormat = pixelFormat,
                .colorSpace = colorSpace
            });
        }
    }
    return surfaceFormats;
}

std::set<PresentMode> MetalSurface::supportedPresentModes(const Device&) const
{
    return {
        PresentMode::fifo
    };
}

}
