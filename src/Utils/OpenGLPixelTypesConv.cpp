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

utils::uint64 toOpenGLPixelFormat(PixelFormat pxFormat)
{
    switch (pxFormat)
    {
    case PixelFormat::RGBA:
        return GL_RGBA;
    case PixelFormat::BGRA:
        return GL_BGRA;
    }
}
PixelFormat fromOpenGLPixelFormat(utils::uint64 glPxFormat)
{
    switch (glPxFormat)
    {
    case GL_RGBA:
        return PixelFormat::RGBA;

    case GL_BGRA:
        return PixelFormat::BGRA;

    default:
        throw utils::RuntimeError("not implemented");
    }
}

}