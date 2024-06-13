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

#include "Graphics/Enums.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Graphics/Window.hpp"
#include "UtilsCPP/Types.hpp"
#include "UtilsCPP/UniquePtr.hpp"
#include "Window/OpenGLWindow.hpp"
#ifdef GFX_IMGUI_ENABLED
    #include "imgui/imgui.h"
#endif

namespace gfx
{

class OpenGLGraphicAPI : public GraphicAPI
{
public:
    OpenGLGraphicAPI()                        = delete;
    OpenGLGraphicAPI(const OpenGLGraphicAPI&) = delete;
    OpenGLGraphicAPI(OpenGLGraphicAPI&&)      = delete;

    OpenGLGraphicAPI(const utils::SharedPtr<Window>&);
    
    #ifdef GFX_IMGUI_ENABLED
    void useForImGui(ImGuiConfigFlags flags = 0) override;
    #endif

    #ifdef GFX_METAL_ENABLED
    inline void initMetalShaderLib(const utils::String& path) override {}
    #endif

    utils::SharedPtr<VertexBuffer> newVertexBuffer(void* data, utils::uint64 count, utils::uint32 vertexSize, const utils::Array<VertexBuffer::LayoutElement>& layout) const override;
    utils::SharedPtr<GraphicPipeline> newGraphicsPipeline(const GraphicPipeline::Descriptor&) const override;
    utils::SharedPtr<IndexBuffer> newIndexBuffer(const utils::Array<utils::uint32>& indices) const override;
    utils::SharedPtr<Texture> newTexture(const Texture::Descriptor&) const override;
    utils::SharedPtr<FrameBuffer> newFrameBuffer(const utils::SharedPtr<Texture>& colorTexture = utils::SharedPtr<Texture>()) const override;

    void beginFrame() override;

    inline void setLoadAction(LoadAction act) override { m_nextPassLoadAction = act; }
    inline void setClearColor(math::rgba col) override { m_nextPassClearColor = col; }
    
    void beginOnScreenRenderPass() override;
    void beginOffScreenRenderPass(const utils::SharedPtr<FrameBuffer>&) override;

    void useGraphicsPipeline(const utils::SharedPtr<GraphicPipeline>&) override;
    void useVertexBuffer(const utils::SharedPtr<VertexBuffer>&) override;

    void setVertexUniform(utils::uint32 index, const math::vec4f&) override;
    void setVertexUniform(utils::uint32 index, const math::mat4x4&) override;
    void setVertexUniform(utils::uint32 index, const math::vec2f&) override;
    void setVertexUniform(utils::uint32 index, const math::mat3x3&) override;
    
    void setFragmentUniform(utils::uint32 index, const math::vec4f&) override;
    void setFragmentTexture(utils::uint32 index, const utils::SharedPtr<Texture>&) override;
    
    void drawVertices(utils::uint32 start, utils::uint32 count) override;
    void drawIndexedVertices(const utils::SharedPtr<IndexBuffer>&) override;

    void endOnScreenRenderPass() override;
    void endOffScreeRenderPass() override;

    void endFrame() override;

    ~OpenGLGraphicAPI() override;

private:
    utils::SharedPtr<OpenGLWindow> m_window;
    
    //pass time
    LoadAction m_nextPassLoadAction = LoadAction::clear; 
    math::rgba m_nextPassClearColor = BLACK; 
    utils::uint32 m_nextTextureUnit = 0;
    utils::Array<utils::UniquePtr<utils::SharedPtrBase>> m_passObjects;

public:
    OpenGLGraphicAPI& operator = (const OpenGLGraphicAPI&) = delete;
    OpenGLGraphicAPI& operator = (OpenGLGraphicAPI&&)      = delete;
};

}

#endif // OPENGLGRAPHICAPI_HPP