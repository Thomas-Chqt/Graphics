/*
 * ---------------------------------------------------
 * MetalPixelTypesConv.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/12 19:48:50
 * ---------------------------------------------------
 */

#include "Graphics/Enums.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/Types.hpp"

#import <Metal/Metal.h>

namespace gfx
{

utils::uint64 toMetalPixelFormat(ColorPixelFormat pxFormat)
{
    switch (pxFormat)
    {
    case ColorPixelFormat::RGBA:
        return MTLPixelFormatRGBA8Unorm;
    case ColorPixelFormat::BGRA:
        return MTLPixelFormatBGRA8Unorm;
    }
}

utils::uint64 toMetalPixelFormat(DepthPixelFormat pxFormat)
{
    switch (pxFormat)
    {
    case DepthPixelFormat::Depth32:
        return MTLPixelFormatDepth32Float;
    }
}

ColorPixelFormat fromMetalPixelFormat(utils::uint64 mtlPxFormat)
{
    switch (mtlPxFormat)
    {
    case MTLPixelFormatRGBA8Unorm:
        return ColorPixelFormat::RGBA;

    case MTLPixelFormatBGRA8Unorm:
        return ColorPixelFormat::BGRA;

    default:
        throw utils::RuntimeError("not implemented");
    }
}

}