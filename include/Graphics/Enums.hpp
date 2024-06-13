
/*
 * ---------------------------------------------------
 * Enums.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/11 15:00:10
 * ---------------------------------------------------
 */

#ifndef ENUMS_HPP
# define ENUMS_HPP

#include "UtilsCPP/Types.hpp" // IWYU pragma: keep

namespace gfx
{

enum class PixelFormat    { RGBA, BGRA };
enum class BlendOperation { blendingOff, srcA_plus_1_minus_srcA, one_minus_srcA_plus_srcA };
enum class Type           { FLOAT };
enum class LoadAction     { load, clear };

#ifdef GFX_METAL_ENABLED
    utils::uint64 toMetalPixelFormat(PixelFormat pxFormat);
    PixelFormat fromMetalPixelFormat(utils::uint64 mtlPxFormat);
#endif
#ifdef GFX_OPENGL_ENABLED
    utils::uint64 toOpenGLPixelFormat(PixelFormat pxFormat);
    PixelFormat fromOpenGLPixelFormat(utils::uint64 glPxFormat);
#endif

}

#endif // ENUMS_HPP
