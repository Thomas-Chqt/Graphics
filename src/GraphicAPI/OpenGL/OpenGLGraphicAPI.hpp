/*
 * ---------------------------------------------------
 * OpenGLGraphicAPI.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/19 14:42:15
 * ---------------------------------------------------
 */

#ifndef OPENGLGRAPHICAPI_HPP
# define OPENGLGRAPHICAPI_HPP

#include "GraphicAPI/OpenGL/OpenGLBuffer.hpp"
#include "GraphicAPI/OpenGL/OpenGLFrameBuffer.hpp"
#include "GraphicAPI/OpenGL/OpenGLGraphicPipeline.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Graphics/Window.hpp"
#include "UtilsCPP/Types.hpp"
#include "Window/OpenGLWindow.hpp"

namespace gfx
{

class OpenGLGraphicAPI : public GraphicAPI
{
public:
    OpenGLGraphicAPI()                        = delete;
    OpenGLGraphicAPI(const OpenGLGraphicAPI&) = delete;
    OpenGLGraphicAPI(OpenGLGraphicAPI&&)      = delete;

    OpenGLGraphicAPI(const utils::SharedPtr<Window>&);
    
    utils::SharedPtr<Shader> newShader(const Shader::MetalShaderDescriptor&, const Shader::OpenGLShaderDescriptor&) const override;
    utils::SharedPtr<GraphicPipeline> newGraphicsPipeline(const GraphicPipeline::Descriptor&) const override;
    utils::SharedPtr<Buffer> newBuffer(const Buffer::Descriptor&) const override;
    utils::SharedPtr<Texture> newTexture(const Texture::Descriptor&) const override;
    utils::SharedPtr<FrameBuffer> newFrameBuffer(const utils::SharedPtr<Texture>& colorTexture = utils::SharedPtr<Texture>()) const override;

    void beginFrame() override;

    inline void setLoadAction(LoadAction act) override { m_nextPassLoadAction = act; }
    inline void setClearColor(math::rgba col) override { m_nextPassClearColor = col; }

    void beginRenderPass() override;
    void beginRenderPass(const utils::SharedPtr<FrameBuffer>&) override;

    void useGraphicsPipeline(const utils::SharedPtr<GraphicPipeline>&) override;
    void useVertexBuffer(const utils::SharedPtr<Buffer>&) override;

    void drawVertices(utils::uint32 start, utils::uint32 count) override;
    void drawIndexedVertices(const utils::SharedPtr<Buffer>&) override;

    void endRenderPass() override;

    void endFrame() override;

    ~OpenGLGraphicAPI() override = default;

private:
    // Life time
    utils::SharedPtr<OpenGLWindow> m_window;

    // pass description
    LoadAction m_nextPassLoadAction = LoadAction::clear;
    math::rgba m_nextPassClearColor = BLACK;

    //pass time
    utils::SharedPtr<OpenGLFrameBuffer> m_frameBuffer;
    utils::SharedPtr<OpenGLGraphicPipeline> m_graphicPipeline;
    utils::SharedPtr<OpenGLBuffer> m_vertextBuffer;
    utils::SharedPtr<OpenGLBuffer> m_indexBuffer;

public:
    OpenGLGraphicAPI& operator = (const OpenGLGraphicAPI&) = delete;
    OpenGLGraphicAPI& operator = (OpenGLGraphicAPI&&)      = delete;
};

}

#endif // OPENGLGRAPHICAPI_HPP