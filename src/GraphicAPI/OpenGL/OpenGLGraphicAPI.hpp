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
#include "Math/Vector.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Graphics/Window.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"
#include "UtilsCPP/UniquePtr.hpp"
#include "Window/OpenGLWindow.hpp"

namespace gfx
{

class OpenGLGraphicAPI : public GraphicAPI
{
private:
    friend utils::SharedPtr<GraphicAPI> Platform::newOpenGLGraphicAPI(const utils::SharedPtr<Window>& renderTarget);

public:
    OpenGLGraphicAPI()                        = delete;
    OpenGLGraphicAPI(const OpenGLGraphicAPI&) = delete;
    OpenGLGraphicAPI(OpenGLGraphicAPI&&)      = delete;

    void setRenderTarget(const utils::SharedPtr<Window>&) override;

#ifdef IMGUI_ENABLED
    void useForImGui(const utils::Func<void()>& f = utils::Func<void()>()) override;
#endif

    inline void setClearColor(const math::rgba& color) override { m_clearColor = color; }

    utils::SharedPtr<VertexBuffer> newVertexBuffer(void* data, utils::uint64 size, const VertexBuffer::LayoutBase& layout) override;
    utils::SharedPtr<GraphicPipeline> newGraphicsPipeline(const utils::String& vertexShaderName, const utils::String& fragmentShaderName, GraphicPipeline::BlendingOperation = GraphicPipeline::BlendingOperation::srcA_plus_1_minus_srcA) override;
    utils::SharedPtr<IndexBuffer> newIndexBuffer(const utils::Array<utils::uint32>& indices) override;
    utils::SharedPtr<Texture> newTexture(utils::uint32 width, utils::uint32 height, Texture::PixelFormat = Texture::PixelFormat::RGBA) override;

    void beginFrame(bool clearBuffer = true) override;

    void useGraphicsPipeline(const utils::SharedPtr<GraphicPipeline>&) override;
    void useVertexBuffer(const utils::SharedPtr<VertexBuffer>&) override;

    void setVertexUniform(utils::uint32 index, const math::vec4f& vec) override;
    void setVertexUniform(utils::uint32 index, const math::mat4x4& mat) override;
    void setVertexUniform(utils::uint32 index, const math::vec2f&) override;
    void setVertexUniform(utils::uint32 index, const math::mat3x3& mat) override;
    
    void setFragmentUniform(utils::uint32 index, const math::vec4f& vec) override;
    void setFragmentTexture(utils::uint32 index, const utils::SharedPtr<Texture>&) override;
    
    void drawVertices(utils::uint32 start, utils::uint32 count) override;
    void drawIndexedVertices(const utils::SharedPtr<IndexBuffer>&) override;
    
    void endFrame() override;

    ~OpenGLGraphicAPI() override;

private:
    OpenGLGraphicAPI(const utils::SharedPtr<Window>& renderTarget);

    utils::SharedPtr<OpenGLWindow> m_renderTarget;
    math::rgba m_clearColor = BLACK;
    utils::uint32 m_nextTextureUnit = 0;

    utils::Array<utils::UniquePtr<utils::SharedPtrBase>> m_frameObjects;

public:
    OpenGLGraphicAPI& operator = (const OpenGLGraphicAPI&) = delete;
    OpenGLGraphicAPI& operator = (OpenGLGraphicAPI&&)      = delete;
};

}

#endif // OPENGLGRAPHICAPI_HPP