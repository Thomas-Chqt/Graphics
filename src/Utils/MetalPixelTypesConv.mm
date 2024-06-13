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

utils::uint64 toMetalPixelFormat(PixelFormat pxFormat)
{
    switch (pxFormat)
    {
    case PixelFormat::RGBA:
        return MTLPixelFormatRGBA8Unorm;
    case PixelFormat::BGRA:
        return MTLPixelFormatBGRA8Unorm;
    }
}
PixelFormat fromMetalPixelFormat(utils::uint64 mtlPxFormat)
{
    switch (mtlPxFormat)
    {
    case MTLPixelFormatRGBA8Unorm:
        return PixelFormat::RGBA;

    case MTLPixelFormatBGRA8Unorm:
        return PixelFormat::BGRA;

    default:
        throw utils::RuntimeError("not implemented");
    }
}

}