/*
 * ---------------------------------------------------
 * OpenGLShader.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/09 16:59:35
 * ---------------------------------------------------
 */

#include "GraphicAPI/OpenGL/OpenGLShader.hpp"
#include "Graphics/Error.hpp"

#define GL_CALL(x) { x; GLenum __err__ = glGetError(); if (__err__ != GL_NO_ERROR) throw OpenGLCallError(__err__); }

namespace gfx
{

OpenGLShader::OpenGLShader(const Shader::Descriptor& descriptor) : m_shaderType(descriptor.type)
{
    int success = 1;
    char errorLog[1024];

    switch (descriptor.type)
    {
    case ShaderType::vertex:
        GL_CALL(m_shaderID = glCreateShader(GL_VERTEX_SHADER))
        break;
    case ShaderType::fragment:
        GL_CALL(m_shaderID = glCreateShader(GL_FRAGMENT_SHADER))
        break;
    }
    const GLchar *const shaderSRC = descriptor.openglCode;
    GL_CALL(glShaderSource(m_shaderID, 1, &shaderSRC, nullptr))

    GL_CALL(glCompileShader(m_shaderID))

    GL_CALL(glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &success))
    if (success == 0)
    {
        GL_CALL(glGetShaderInfoLog(m_shaderID, 1024, nullptr, &errorLog[0]))
        throw OpenGLShaderCompileError((char*)errorLog);
    }
}

OpenGLShader::~OpenGLShader()
{
    glDeleteShader(m_shaderID);
}

}