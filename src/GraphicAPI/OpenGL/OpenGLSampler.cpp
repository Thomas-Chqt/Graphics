/*
 * ---------------------------------------------------
 * OpenGLSampler.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/12 13:36:39
 * ---------------------------------------------------
 */

#include "GraphicAPI/OpenGL/OpenGLSampler.hpp"
#include "Graphics/Enums.hpp"

namespace gfx
{

OpenGLSampler::OpenGLSampler(const Sampler::Descriptor& descriptor)
    : m_sAddressMode(toOpenGLSamplerAddressMode(descriptor.sAddressMode)),
      m_tAddressMode(toOpenGLSamplerAddressMode(descriptor.tAddressMode)),
      m_minFilter(toOpenGLSamplerMinMagFilter(descriptor.minFilter)),
      m_magFilter(toOpenGLSamplerMinMagFilter(descriptor.magFilter))
{
}

}