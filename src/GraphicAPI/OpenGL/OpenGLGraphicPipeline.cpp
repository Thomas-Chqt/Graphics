/*
 * ---------------------------------------------------
 * OpenGLGraphicPipeline.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/20 17:41:41
 * ---------------------------------------------------
 */

#include "GraphicAPI/OpenGL/OpenGLGraphicPipeline.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/ShaderLibrary.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"
#include <cassert>

using utils::uint32;
using utils::String;

namespace gfx
{

uint32 OpenGLGraphicPipeline::findVertexUniformIndex(const String& name)
{
    return (uint32)glGetUniformLocation(m_shaderProgramID, (const char*)name);
}

uint32 OpenGLGraphicPipeline::findFragmentUniformIndex(const String& name)
{
    return (uint32)glGetUniformLocation(m_shaderProgramID, (const char*)name);
}

OpenGLGraphicPipeline::~OpenGLGraphicPipeline()
{
    glDeleteProgram(m_shaderProgramID);
}

OpenGLGraphicPipeline::OpenGLGraphicPipeline(const utils::String& vertexShaderName, const utils::String& fragmentShaderName, GraphicPipeline::BlendingOperation operation)
{
    int success;
    char errorLog[1024];

    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    const GLchar *const vertexShaderSRC = ShaderLibrary::shared().getGlslCode(vertexShaderName);
    glShaderSource(vertexShaderID, 1, &vertexShaderSRC, nullptr);
    glCompileShader(vertexShaderID);
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
    if (success == 0)
    {
        glGetShaderInfoLog(vertexShaderID, 1024, nullptr, &errorLog[0]);
        throw OpenGLShaderCompileError(errorLog);
    }

    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar *const fragmentShaderSRC = ShaderLibrary::shared().getGlslCode(fragmentShaderName);
    glShaderSource(fragmentShaderID, 1, &fragmentShaderSRC, nullptr);
    glCompileShader(fragmentShaderID);
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
    if (success == 0)
    {
        glGetShaderInfoLog(fragmentShaderID, 1024, nullptr, &errorLog[0]);
        throw OpenGLShaderCompileError(errorLog);
    }

    m_shaderProgramID = glCreateProgram();
    glAttachShader(m_shaderProgramID, vertexShaderID);
    glAttachShader(m_shaderProgramID, fragmentShaderID);
    glLinkProgram(m_shaderProgramID);
    glGetProgramiv(m_shaderProgramID, GL_LINK_STATUS, &success);
    if (success == 0)
    {
        glGetProgramInfoLog(m_shaderProgramID, 1024, nullptr, &errorLog[0]);
        throw OpenGLShaderCompileError(errorLog);
    }

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    switch (operation)
    {
    case GraphicPipeline::BlendingOperation::srcA_plus_1_minus_srcA:
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
        break;

    case GraphicPipeline::BlendingOperation::one_minus_srcA_plus_srcA:
        glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
        break;
    }
}

}