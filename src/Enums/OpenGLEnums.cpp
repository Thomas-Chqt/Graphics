/*
 * ---------------------------------------------------
 * OpenGLEnums.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/13 11:34:03
 * ---------------------------------------------------
 */

#include "Graphics/Enums.hpp"
#include "UtilsCPP/Types.hpp"
#include "UtilsCPP/Macros.hpp"

#include <glad/glad.h>

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
    case PixelFormat::Depth32:
        return GL_DEPTH_COMPONENT;
    }
    UNREACHABLE
}

utils::uint32 toOpenGLInternalPixelFormat(PixelFormat pxFormat)
{
    switch (pxFormat)
    {
    case PixelFormat::RGBA:
    case PixelFormat::BGRA:
        return GL_RGBA;
    case PixelFormat::Depth32:
        return GL_DEPTH_COMPONENT;
    }
    UNREACHABLE
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
    UNREACHABLE
}

utils::uint32 toOpenGLVertexAttributeFormatType(VertexAttributeFormat format)
{
    switch (format)
    {
    case VertexAttributeFormat::vec2f:
    case VertexAttributeFormat::vec3f:
        return GL_FLOAT;
    }
    UNREACHABLE
}

utils::uint8 toOpenGLVertexAttributeFormatNormalized(VertexAttributeFormat format)
{
    switch (format)
    {
    case VertexAttributeFormat::vec2f:
    case VertexAttributeFormat::vec3f:
        return GL_FALSE;
    }
    UNREACHABLE
}

}
