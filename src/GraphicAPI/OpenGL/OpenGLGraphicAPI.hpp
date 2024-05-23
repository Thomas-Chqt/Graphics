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
#include "Graphics/Platform.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Graphics/Window.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"
#include "Window/OpenGLWindow.hpp"

namespace gfx
{

class OpenGLGraphicAPI : public GraphicAPI
{
private:
    friend utils::SharedPtr<GraphicAPI> Platform::newOpenGLGraphicAPI(const utils::SharedPtr<Window>& renderTarget);

public:
    OpenGLGraphicAPI(const OpenGLGraphicAPI&) = delete;
    OpenGLGraphicAPI(OpenGLGraphicAPI&&)      = delete;

    void setRenderTarget(const utils::SharedPtr<Window>&) override;

#ifdef IMGUI_ENABLED
    void useForImGui() override;
#endif

    void setClearColor(float r, float g, float b, float a) override;

    utils::SharedPtr<VertexBuffer> newVertexBuffer(void* data, utils::uint64 size, const VertexBuffer::LayoutBase& layout) override;
    utils::SharedPtr<GraphicPipeline> newGraphicsPipeline(const utils::String& vertexShaderName, const utils::String& fragmentShaderName) override;
    utils::SharedPtr<IndexBuffer> newIndexBuffer(const utils::Array<utils::uint32>& indices) override;

    void beginFrame() override;    

    void useGraphicsPipeline(utils::SharedPtr<GraphicPipeline>) override;
    void useVertexBuffer(utils::SharedPtr<VertexBuffer>) override;
    
    void drawVertices(utils::uint32 start, utils::uint32 count) override;
    void drawIndexedVertices(utils::SharedPtr<IndexBuffer>) override;
    
    void endFrame() override;

    ~OpenGLGraphicAPI() override;

private:
    OpenGLGraphicAPI(const utils::SharedPtr<Window>& renderTarget);

    utils::SharedPtr<OpenGLWindow> m_renderTarget;
    float m_clearColor[4] = {};

public:
    OpenGLGraphicAPI& operator = (const OpenGLGraphicAPI&) = delete;
    OpenGLGraphicAPI& operator = (OpenGLGraphicAPI&&)      = delete;
};

}

#endif // OPENGLGRAPHICAPI_HPP