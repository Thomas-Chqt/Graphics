/*
 * ---------------------------------------------------
 * OpenGLSampler.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/12 13:27:18
 * ---------------------------------------------------
 */

#ifndef OPENGLSAMPLER_HPP
# define OPENGLSAMPLER_HPP

#include "Graphics/Sampler.hpp"
#include <GL/glew.h>

namespace gfx
{

class OpenGLSampler : public Sampler
{
public:
    OpenGLSampler()                     = delete;
    OpenGLSampler(const OpenGLSampler&) = delete;
    OpenGLSampler(OpenGLSampler&&)      = delete;
    
    OpenGLSampler(const Sampler::Descriptor&);

    inline GLint sAddressMode() const { return m_sAddressMode; };
    inline GLint tAddressMode() const { return m_tAddressMode; };
    inline GLint rAddressMode() const { return m_rAddressMode; };
    inline GLint minFilter() const { return m_minFilter; };
    inline GLint magFilter() const { return m_magFilter; };

    ~OpenGLSampler() = default;

private:
    GLint m_sAddressMode = 0;
    GLint m_tAddressMode = 0;
    GLint m_rAddressMode = 0;
    GLint m_minFilter = 0;
    GLint m_magFilter = 0;

public:
    OpenGLSampler& operator = (const OpenGLSampler&) = delete;
    OpenGLSampler& operator = (OpenGLSampler&&)      = delete;
};

}

#endif // OPENGLSAMPLER_HPP