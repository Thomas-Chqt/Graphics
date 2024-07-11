/*
 * ---------------------------------------------------
 * OpenGLShader.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/09 16:49:40
 * ---------------------------------------------------
 */

#ifndef OPENGLSHADER_HPP
# define OPENGLSHADER_HPP

#include "Graphics/Enums.hpp"
#include "Graphics/Shader.hpp"
#include <GL/glew.h>

namespace gfx
{

class OpenGLShader : public Shader
{
public:
    OpenGLShader()                    = delete;
    OpenGLShader(const OpenGLShader&) = delete;
    OpenGLShader(OpenGLShader&&)      = delete;
    
    explicit OpenGLShader(const Shader::OpenGLShaderDescriptor&);

    inline ShaderType type() override { return m_shaderType; }

    GLuint shaderID() const { return m_shaderID; }

    ~OpenGLShader() override;

private:
    GLuint m_shaderID;
    const ShaderType m_shaderType;

public:
    OpenGLShader& operator = (const OpenGLShader&) = delete;
    OpenGLShader& operator = (OpenGLShader&&)      = delete;
};

}

#endif // OPENGLSHADER_HPP