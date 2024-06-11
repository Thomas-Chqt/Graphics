
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

namespace gfx
{

enum class PixelFormat { RGBA, ARGB };
enum class BlendOperation { blendingOff, srcA_plus_1_minus_srcA, one_minus_srcA_plus_srcA };
enum class Type { FLOAT };
enum class LoadAction { load, clear };

}

#endif // ENUMS_HPP
