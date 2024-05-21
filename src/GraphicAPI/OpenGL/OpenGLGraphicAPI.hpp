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

#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Graphics/Window.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"

namespace gfx
{

class OpenGLGraphicAPI : public GraphicAPI
{
private:
    friend utils::SharedPtr<GraphicAPI> GraphicAPI::newOpenGLGraphicAPI(const utils::SharedPtr<Window>& renderTarget);

public:
    OpenGLGraphicAPI(const OpenGLGraphicAPI&) = delete;
    OpenGLGraphicAPI(OpenGLGraphicAPI&&)      = delete;

    utils::SharedPtr<VertexBuffer> newVertexBuffer(void* data, utils::uint64 size, const VertexBuffer::LayoutBase& layout) override;
    utils::SharedPtr<GraphicPipeline> newGraphicsPipeline(const utils::String& vertexShaderName, const utils::String& fragmentShaderName) override;

    void setRenderTarget(const utils::SharedPtr<Window>&) override;

    void setClearColor(float r, float g, float b, float a) override;

    void beginFrame() override;    

    void useGraphicsPipeline(utils::SharedPtr<GraphicPipeline>) override;
    void useVertexBuffer(utils::SharedPtr<VertexBuffer>) override;
    
    void drawVertices(utils::uint32 start, utils::uint32 count) override;
    
    void endFrame() override;

    ~OpenGLGraphicAPI() override;

private:
    OpenGLGraphicAPI(const utils::SharedPtr<Window>& renderTarget);

    utils::SharedPtr<Window> m_renderTarget;
    float m_clearColor[4] = {};

public:
    OpenGLGraphicAPI& operator = (const OpenGLGraphicAPI&) = delete;
    OpenGLGraphicAPI& operator = (OpenGLGraphicAPI&&)      = delete;
};

}

#endif // OPENGLGRAPHICAPI_HPP