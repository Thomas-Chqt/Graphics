
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

enum class ColorPixelFormat { RGBA, BGRA };
enum class DepthPixelFormat { Depth32 };
enum class BlendOperation   { blendingOff, srcA_plus_1_minus_srcA, one_minus_srcA_plus_srcA };
enum class Type             { Float, vec2f, vec3f };
enum class LoadAction       { load, clear };

#ifdef GFX_METAL_ENABLED
    utils::uint64 toMetalPixelFormat(ColorPixelFormat pxFormat);
    utils::uint64 toMetalPixelFormat(DepthPixelFormat pxFormat);
    ColorPixelFormat fromMetalPixelFormat(utils::uint64 mtlPxFormat);
#endif
#ifdef GFX_OPENGL_ENABLED
    utils::uint64 toOpenGLPixelFormat(ColorPixelFormat pxFormat);
    ColorPixelFormat fromOpenGLPixelFormat(utils::uint64 glPxFormat);
#endif

}

#endif // ENUMS_HPP
