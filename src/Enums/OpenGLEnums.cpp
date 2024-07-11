/*
 * ---------------------------------------------------
 * OpenGLEnums.cpp
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

utils::uint32 toOpenGLPixelFormat(PixelFormat pxFormat)
{
    switch (pxFormat)
    {
    case PixelFormat::RGBA:
        return GL_RGBA;
    case PixelFormat::BGRA:
        return GL_BGRA;
    default:
        throw utils::RuntimeError("not implemented");
    }
}

PixelFormat fromOpenGLPixelFormat(utils::uint32 glPxFormat)
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

int toOpenGLVertexAttributeFormatSize(VertexAttributeFormat format)
{
    switch (format)
    {
    case VertexAttributeFormat::vec2f:
        return 2;
    case VertexAttributeFormat::vec3f:
        return 3;
    }
}

utils::uint32 toOpenGLVertexAttributeFormatType(VertexAttributeFormat format)
{
    switch (format)
    {
    case VertexAttributeFormat::vec2f:
    case VertexAttributeFormat::vec3f:
        return GL_FLOAT;
    }
}

utils::uint8 toOpenGLVertexAttributeFormatNormalized(VertexAttributeFormat format)
{
    switch (format)
    {
    case VertexAttributeFormat::vec2f:
    case VertexAttributeFormat::vec3f:
        return GL_FALSE;
    }
}

}
