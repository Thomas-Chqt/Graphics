/*
 * ---------------------------------------------------
 * MetalEnums.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/02 07:22:09
 * ---------------------------------------------------
 */

#include "Graphics/Enums.hpp"

#include "Metal/MetalEnums.hpp"

#import <Metal/Metal.h>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <stdexcept>
    namespace ext = std;
#endif

namespace gfx
{

MTLPixelFormat toMTLPixelFormat(PixelFormat pxf)
{
    switch (pxf)
    {
    case PixelFormat::BGRA8Unorm:
        return MTLPixelFormatBGRA8Unorm;
    case PixelFormat::BGRA8Unorm_sRGB:
        return MTLPixelFormatBGRA8Unorm_sRGB;
    default:
        throw ext::runtime_error("not implemented");
    }
}

MTLLoadAction toMTLLoadAction(LoadAction lac)
{
    switch (lac)
    {
    case LoadAction::load:
        return MTLLoadActionLoad;
    case LoadAction::clear:
        return MTLLoadActionClear;
    default:
        throw ext::runtime_error("not implemented");
    }
}

}
