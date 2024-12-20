
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

#include "UtilsCPP/Types.hpp"

namespace gfx
{

enum class PixelFormat           { RGBA, BGRA, Depth32 };
enum class LoadAction            { load, clear };
enum class ShaderType            { vertex, fragment };
enum class BlendOperation        { blendingOff, srcA_plus_1_minus_srcA, one_minus_srcA_plus_srcA };
enum class VertexAttributeFormat { vec2f, vec3f };
enum class StorageMode           { Private, Shared, Managed };

#ifdef GFX_BUILD_METAL
    utils::uint64 toMetalPixelFormat(PixelFormat);
    utils::uint64 toMetalVertexAttributeFormat(VertexAttributeFormat);
#endif
#ifdef GFX_BUILD_OPENGL
    utils::uint32 toOpenGLPixelFormat(PixelFormat);
    utils::uint32 toOpenGLInternalPixelFormat(PixelFormat);
    int toOpenGLVertexAttributeFormatSize(VertexAttributeFormat);
    utils::uint32 toOpenGLVertexAttributeFormatType(VertexAttributeFormat);
    utils::uint8 toOpenGLVertexAttributeFormatNormalized(VertexAttributeFormat);
#endif

}

#endif // ENUMS_HPP
