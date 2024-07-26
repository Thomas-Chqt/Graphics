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
#include "UtilsCPP/Dictionary.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"
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

    inline utils::uint64 getVertexBufferIndex(const utils::String& name) override { return (utils::uint64)getBufferBindingPoint(name); }
    inline utils::uint64 getVertexTextureIndex(const utils::String& name) override { return getSamplerUniformIndex(name); }
    inline utils::uint64 getVertexSamplerIndex(const utils::String&) override { return 0; }

    inline utils::uint64 getFragmentBufferIndex(const utils::String& name) override { return (utils::uint64)getBufferBindingPoint(name); }
    inline utils::uint64 getFragmentTextureIndex(const utils::String& name) override { return getSamplerUniformIndex(name); }
    inline utils::uint64 getFragmentSamplerIndex(const utils::String&) override { return 0; }


    inline GLuint shaderProgramID() const { return m_shaderProgramID; }
    void enableVertexLayout();
    inline BlendOperation blendOperation() const { return m_descriptor.blendOperation; }
    
    ~OpenGLGraphicPipeline() override;

private:
    inline utils::uint32 getBufferBindingPoint(const utils::String& name) { return m_unifomBufferBindingPoints[name]; }
    utils::uint32 getSamplerUniformIndex(const utils::String& name) const;

    GLuint m_shaderProgramID = 0;
    GLuint m_vertexArrayID = 0;
    GraphicPipeline::Descriptor m_descriptor;
    utils::Dictionary<utils::String, utils::uint32> m_unifomBufferBindingPoints;

public:
    OpenGLGraphicPipeline& operator = (const OpenGLGraphicPipeline&) = delete;
    OpenGLGraphicPipeline& operator = (OpenGLGraphicPipeline&&)      = delete;
};

}

#endif // OPENGLGRAPHICSPIPELINE_HPP