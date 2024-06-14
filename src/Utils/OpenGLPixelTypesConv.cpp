/*
 * ---------------------------------------------------
 * OpenGLPixelTypesConv.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/13 11:34:03
 * ---------------------------------------------------
 */

#include "Graphics/Enums.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/Types.hpp"

#include "GL/glew.h"

namespace gfx
{

utils::uint64 toOpenGLPixelFormat(ColorPixelFormat pxFormat)
{
    switch (pxFormat)
    {
    case ColorPixelFormat::RGBA:
        return GL_RGBA;
    case ColorPixelFormat::BGRA:
        return GL_BGRA;
    }
}
ColorPixelFormat fromOpenGLPixelFormat(utils::uint64 glPxFormat)
{
    switch (glPxFormat)
    {
    case GL_RGBA:
        return ColorPixelFormat::RGBA;

    case GL_BGRA:
        return ColorPixelFormat::BGRA;

    default:
        throw utils::RuntimeError("not implemented");
    }
}

}