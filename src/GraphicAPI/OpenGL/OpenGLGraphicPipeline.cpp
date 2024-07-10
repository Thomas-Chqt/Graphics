/*
 * ---------------------------------------------------
 * OpenGLGraphicPipeline.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/20 17:41:41
 * ---------------------------------------------------
 */

#include "GraphicAPI/OpenGL/OpenGLGraphicPipeline.hpp"
#include "GraphicAPI/OpenGL/OpenGLShader.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "UtilsCPP/String.hpp"
#include "Graphics/Error.hpp"
#include "UtilsCPP/Types.hpp"

#define GL_CALL(x) { x; GLenum __err__; if ((__err__ = glGetError()) != GL_NO_ERROR) throw OpenGLCallError(__err__); }
#define TO_OPENGL_VERTEX_ATTRIBUTE_FORMAT(frmt) toOpenGLVertexAttributeFormatSize(frmt), toOpenGLVertexAttributeFormatType(frmt), toOpenGLVertexAttributeFormatNormalized(frmt)

namespace gfx
{

OpenGLGraphicPipeline::OpenGLGraphicPipeline(const GraphicPipeline::Descriptor& descriptor) : m_descriptor(descriptor)
{
    int success;
    char errorLog[1024];

    OpenGLShader& vertexShader = dynamic_cast<OpenGLShader&>(*m_descriptor.vertexShader);
    OpenGLShader& fragmentShader = dynamic_cast<OpenGLShader&>(*m_descriptor.fragmentShader);

    GL_CALL(m_shaderProgramID = glCreateProgram());
    GL_CALL(glAttachShader(m_shaderProgramID, vertexShader.shaderID()));
    GL_CALL(glAttachShader(m_shaderProgramID, fragmentShader.shaderID()));
    GL_CALL(glLinkProgram(m_shaderProgramID));
    GL_CALL(glGetProgramiv(m_shaderProgramID, GL_LINK_STATUS, &success));
    if (success == 0)
    {
        GL_CALL(glGetProgramInfoLog(m_shaderProgramID, 1024, nullptr, &errorLog[0]));
        throw OpenGLShaderLinkError(errorLog);
    }

    GL_CALL(glDetachShader(m_shaderProgramID, vertexShader.shaderID()));
    GL_CALL(glDetachShader(m_shaderProgramID, fragmentShader.shaderID()));

    GL_CALL(glGenVertexArrays(1, &m_vertexArrayID))
}

void OpenGLGraphicPipeline::enableVertexLayout()
{
    GL_CALL(glBindVertexArray(m_vertexArrayID));
    for (utils::uint32 i = 0; i < m_descriptor.vertexLayout.attributes.length(); i++)
    {
        const auto& attribute = m_descriptor.vertexLayout.attributes[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, TO_OPENGL_VERTEX_ATTRIBUTE_FORMAT(attribute.format), m_descriptor.vertexLayout.stride, (const void*)attribute.offset);
    }
}

OpenGLGraphicPipeline::~OpenGLGraphicPipeline()
{
    glDeleteVertexArrays(1, &m_vertexArrayID);
    glDeleteProgram(m_shaderProgramID);
}

}