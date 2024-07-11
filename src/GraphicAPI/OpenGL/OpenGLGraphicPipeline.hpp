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

#include "Graphics/Enums.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include <GL/glew.h>

namespace gfx
{

class OpenGLGraphicPipeline : public GraphicPipeline
{
public:
    OpenGLGraphicPipeline()                             = delete;
    OpenGLGraphicPipeline(const OpenGLGraphicPipeline&) = delete;
    OpenGLGraphicPipeline(OpenGLGraphicPipeline&&)      = delete;

    explicit OpenGLGraphicPipeline(GraphicPipeline::Descriptor);

    inline GLuint shaderProgramID() const { return m_shaderProgramID; }
    void enableVertexLayout();
    inline BlendOperation blendOperation() const { return m_descriptor.blendOperation; }
    
    ~OpenGLGraphicPipeline() override;

private:
    GLuint m_shaderProgramID = 0;
    GLuint m_vertexArrayID = 0;
    GraphicPipeline::Descriptor m_descriptor;

public:
    OpenGLGraphicPipeline& operator = (const OpenGLGraphicPipeline&) = delete;
    OpenGLGraphicPipeline& operator = (OpenGLGraphicPipeline&&)      = delete;
};

}

#endif // OPENGLGRAPHICSPIPELINE_HPP