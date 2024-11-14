/*
 * ---------------------------------------------------
 * OpenGLSampler.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/12 13:36:39
 * ---------------------------------------------------
 */

#include "GraphicAPI/OpenGL/OpenGLSampler.hpp"
#include "Graphics/Sampler.hpp"

namespace gfx
{

int toOpenGLSamplerAddressMode(Sampler::AddressMode addressMode)
{
    switch (addressMode)
    {
    case Sampler::AddressMode::ClampToEdge:
        return GL_CLAMP_TO_EDGE;
        break;
    case Sampler::AddressMode::Repeat:
        return GL_REPEAT;
        break;
    case Sampler::AddressMode::MirrorRepeat:
        return GL_MIRRORED_REPEAT;
        break;
    }
    UNREACHABLE
}

int toOpenGLSamplerMinMagFilter(Sampler::MinMagFilter filter)
{
    switch (filter)
    {
    case Sampler::MinMagFilter::Nearest:
        return GL_NEAREST;
        break;
    case Sampler::MinMagFilter::Linear:
        return GL_LINEAR;
        break;
    }
    UNREACHABLE
}


OpenGLSampler::OpenGLSampler(const Sampler::Descriptor& descriptor)
    : m_sAddressMode(toOpenGLSamplerAddressMode(descriptor.sAddressMode)),
      m_tAddressMode(toOpenGLSamplerAddressMode(descriptor.tAddressMode)),
      m_rAddressMode(toOpenGLSamplerAddressMode(descriptor.rAddressMode)),
      m_minFilter(toOpenGLSamplerMinMagFilter(descriptor.minFilter)),
      m_magFilter(toOpenGLSamplerMinMagFilter(descriptor.magFilter))
{
}

}