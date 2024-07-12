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
    case PixelFormat::Depth32:
        return GL_DEPTH_COMPONENT;
    }
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

int toOpenGLSamplerAddressMode(SamplerAddressMode addressMode)
{
    switch (addressMode)
    {
    case SamplerAddressMode::ClampToEdge:
        return GL_CLAMP_TO_EDGE;
        break;
    case SamplerAddressMode::Repeat:
        return GL_REPEAT;
        break;
    case SamplerAddressMode::MirrorRepeat:
        return GL_MIRRORED_REPEAT;
        break;
    }
}

int toOpenGLSamplerMinMagFilter(SamplerMinMagFilter filter)
{
    switch (filter)
    {
    case SamplerMinMagFilter::Nearest:
        return GL_NEAREST;
        break;
    case SamplerMinMagFilter::Linear:
        return GL_LINEAR;
        break;
    }
}


}
