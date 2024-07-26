/*
 * ---------------------------------------------------
 * OpenGLGraphicPipeline.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/20 17:41:41
 * ---------------------------------------------------
 */

#include "GraphicAPI/OpenGL/OpenGLGraphicPipeline.hpp"

#include <utility>
#include "GraphicAPI/OpenGL/OpenGLShader.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/Error.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"

#define GL_CALL(x) { x; GLenum __err__ = glGetError(); if (__err__ != GL_NO_ERROR) throw OpenGLCallError(__err__); }
#define TO_OPENGL_VERTEX_ATTRIBUTE_FORMAT(frmt) toOpenGLVertexAttributeFormatSize(frmt), toOpenGLVertexAttributeFormatType(frmt), toOpenGLVertexAttributeFormatNormalized(frmt)

namespace gfx
{

OpenGLGraphicPipeline::OpenGLGraphicPipeline(GraphicPipeline::Descriptor descriptor) : m_descriptor(std::move(descriptor))
{
    int success = 1;
    char errorLog[1024];

    auto& vertexShader = dynamic_cast<OpenGLShader&>(*m_descriptor.vertexShader);
    auto& fragmentShader = dynamic_cast<OpenGLShader&>(*m_descriptor.fragmentShader);

    GL_CALL(m_shaderProgramID = glCreateProgram())
    GL_CALL(glAttachShader(m_shaderProgramID, vertexShader.shaderID()))
    GL_CALL(glAttachShader(m_shaderProgramID, fragmentShader.shaderID()))
    GL_CALL(glLinkProgram(m_shaderProgramID))
    GL_CALL(glGetProgramiv(m_shaderProgramID, GL_LINK_STATUS, &success))
    if (success == 0)
    {
        GL_CALL(glGetProgramInfoLog(m_shaderProgramID, 1024, nullptr, &errorLog[0]))
        throw OpenGLShaderLinkError((char*)errorLog);
    }

    GL_CALL(glDetachShader(m_shaderProgramID, vertexShader.shaderID()))
    GL_CALL(glDetachShader(m_shaderProgramID, fragmentShader.shaderID()))

    GL_CALL(glGenVertexArrays(1, &m_vertexArrayID))

    GLint uniformBlockCount = 0;
    GL_CALL(glGetProgramiv(m_shaderProgramID, GL_ACTIVE_UNIFORM_BLOCKS, &uniformBlockCount));

    GLuint nextBlockBinding = 1;
    for (GLuint blockIndex = 0; blockIndex < uniformBlockCount; blockIndex++, nextBlockBinding++)
    {
        GLint uniformBlockNameLength = 0;
        GL_CALL(glGetActiveUniformBlockiv(m_shaderProgramID, blockIndex, GL_UNIFORM_BLOCK_NAME_LENGTH, &uniformBlockNameLength));

        utils::String name(uniformBlockNameLength - 1);
        GL_CALL(glGetActiveUniformBlockName(m_shaderProgramID, blockIndex, uniformBlockNameLength, nullptr, &name[0]));

        GL_CALL(glUniformBlockBinding(m_shaderProgramID, blockIndex, nextBlockBinding));

        m_unifomBufferBindingPoints.insert(name, nextBlockBinding);
    }
}

void OpenGLGraphicPipeline::enableVertexLayout()
{
    GL_CALL(glBindVertexArray(m_vertexArrayID))
    for (utils::uint32 i = 0; i < m_descriptor.vertexLayout.attributes.length(); i++)
    {
        const auto& attribute = m_descriptor.vertexLayout.attributes[i];
        GL_CALL(glEnableVertexAttribArray(i))
        GL_CALL(glVertexAttribPointer(i, TO_OPENGL_VERTEX_ATTRIBUTE_FORMAT(attribute.format), (int)m_descriptor.vertexLayout.stride, (const void*)attribute.offset)) // NOLINT(performance-no-int-to-ptr)
    }
}

OpenGLGraphicPipeline::~OpenGLGraphicPipeline()
{
    glDeleteVertexArrays(1, &m_vertexArrayID);
    glDeleteProgram(m_shaderProgramID);
}

utils::uint32 OpenGLGraphicPipeline::getSamplerUniformIndex(const utils::String& name) const
{
    utils::uint32 idx = 0;
    GL_CALL(idx = glGetUniformLocation(m_shaderProgramID, (const char*)name));
    return idx;
}

}