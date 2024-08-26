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
#include "GraphicAPI/OpenGL/OpenGLTexture.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/FrameBuffer.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/Texture.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Graphics/Window.hpp"
#include "UtilsCPP/Types.hpp"
#include "Window/OpenGLWindow.hpp"
#ifdef GFX_BUILD_IMGUI
    #include <imgui.h>
#endif

namespace gfx
{

class OpenGLGraphicAPI : public GraphicAPI
{
public:
    OpenGLGraphicAPI()                        = delete;
    OpenGLGraphicAPI(const OpenGLGraphicAPI&) = delete;
    OpenGLGraphicAPI(OpenGLGraphicAPI&&)      = delete;

    explicit OpenGLGraphicAPI(const utils::SharedPtr<Window>&);
    
    utils::SharedPtr<Shader> newShader(const Shader::Descriptor&) const override;
    utils::SharedPtr<GraphicPipeline> newGraphicsPipeline(const GraphicPipeline::Descriptor&) const override;
    utils::SharedPtr<Buffer> newBuffer(const Buffer::Descriptor&) const override;
    utils::SharedPtr<Texture> newTexture(const Texture::Descriptor&) const override;
    utils::SharedPtr<Sampler> newSampler(const Sampler::Descriptor&) const override;
    utils::SharedPtr<FrameBuffer> newFrameBuffer(const FrameBuffer::Descriptor&) const override;

    #ifdef GFX_BUILD_IMGUI
        void initImgui(ImGuiConfigFlags flags) override;
    #endif

    inline void beginFrame() override {}

    inline void setLoadAction(LoadAction act) override { m_nextPassLoadAction = act; }
    inline void setClearColor(math::rgba col) override { m_nextPassClearColor = col; }

    void beginRenderPass() override;
    void beginRenderPass(const utils::SharedPtr<FrameBuffer>&) override;
    #ifdef GFX_BUILD_IMGUI
        void beginImguiRenderPass() override;
    #endif

    void useGraphicsPipeline(const utils::SharedPtr<GraphicPipeline>&) override;

    void setVertexBuffer(const utils::SharedPtr<Buffer>&, utils::uint64 idx) override;

    void setFragmentBuffer(const utils::SharedPtr<Buffer>&, utils::uint64 idx) override;
    void setFragmentTexture(const utils::SharedPtr<Texture>&, utils::uint64 idx) override;
    void setFragmentTexture(const utils::SharedPtr<Texture>&, utils::uint64 textureIdx, const utils::SharedPtr<Sampler>&, utils::uint64 samplerIdx) override;
 
    void drawVertices(utils::uint32 start, utils::uint32 count) override;
    void drawIndexedVertices(const utils::SharedPtr<Buffer>&) override;

    void endRenderPass() override;

    void endFrame() override;

    #ifdef GFX_BUILD_IMGUI
        void terminateImGui() override;
    #endif

    ~OpenGLGraphicAPI() override = default;

private:
    // Life time
    utils::SharedPtr<OpenGLWindow> m_window;

    //frame time
    #ifdef GFX_BUILD_IMGUI
        bool m_isImguiFrame = false;
    #endif

    // pass description
    LoadAction m_nextPassLoadAction = LoadAction::clear;
    math::rgba m_nextPassClearColor = BLACK;

    //pass time
    #ifdef GFX_BUILD_IMGUI
        bool m_isImguiRenderPass = false;
    #endif
    utils::uint32 m_nextTextureUnit = 0;
    utils::SharedPtr<OpenGLFrameBuffer> m_frameBuffer;
    utils::SharedPtr<OpenGLGraphicPipeline> m_graphicPipeline;
    utils::Dictionary<utils::uint64, utils::SharedPtr<OpenGLBuffer>> m_buffers;
    utils::Dictionary<utils::uint64, utils::SharedPtr<OpenGLTexture>> m_textures;
    utils::SharedPtr<OpenGLBuffer> m_indexBuffer;

public:
    OpenGLGraphicAPI& operator = (const OpenGLGraphicAPI&) = delete;
    OpenGLGraphicAPI& operator = (OpenGLGraphicAPI&&)      = delete;
};

}

#endif // OPENGLGRAPHICAPI_HPP