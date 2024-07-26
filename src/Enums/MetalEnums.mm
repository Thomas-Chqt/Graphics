/*
 * ---------------------------------------------------
 * MetalEnums.mm
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
    case PixelFormat::Depth32:
        return MTLPixelFormatDepth32Float;
    }
}

utils::uint64 toMetalVertexAttributeFormat(VertexAttributeFormat format)
{
    switch (format)
    {
    case VertexAttributeFormat::vec2f:
        return MTLVertexFormatFloat2;
    case VertexAttributeFormat::vec3f:
        return MTLVertexFormatFloat3;
    }
}

}