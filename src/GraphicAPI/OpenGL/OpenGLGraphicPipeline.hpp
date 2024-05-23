/*
 * ---------------------------------------------------
 * OpenGLGraphicPipeline.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/20 17:37:53
 * ---------------------------------------------------
 */

#ifndef OPENGLGRAPHICSPIPELINE_HPP
# define OPENGLGRAPHICSPIPELINE_HPP

#include "GraphicAPI/OpenGL/OpenGLGraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"
#include <GL/glew.h>

namespace gfx
{

class OpenGLGraphicPipeline : public GraphicPipeline
{
private:
    friend utils::SharedPtr<GraphicPipeline> OpenGLGraphicAPI::newGraphicsPipeline(const utils::String& vertexShaderName, const utils::String& fragmentShaderName);

public:
    OpenGLGraphicPipeline()                             = delete;
    OpenGLGraphicPipeline(const OpenGLGraphicPipeline&) = delete;
    OpenGLGraphicPipeline(OpenGLGraphicPipeline&&)      = delete;

    utils::uint32 findVertexUniformIndex(const utils::String& name) override;
    utils::uint32 findFragmentUniformIndex(const utils::String& name) override;

    inline GLuint shaderProgramID() { return m_shaderProgramID; }
    
    ~OpenGLGraphicPipeline();

private:
    OpenGLGraphicPipeline(const utils::String& vertexShaderName, const utils::String& fragmentShaderName);

    GLuint m_shaderProgramID = 0;

public:
    OpenGLGraphicPipeline& operator = (const OpenGLGraphicPipeline&) = delete;
    OpenGLGraphicPipeline& operator = (OpenGLGraphicPipeline&&)      = delete;
};

}

#endif // OPENGLGRAPHICSPIPELINE_HPP