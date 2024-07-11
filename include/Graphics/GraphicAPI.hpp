/*
 * ---------------------------------------------------
 * GraphicAPI.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/19 13:57:02
 * ---------------------------------------------------
 */

#ifndef GRAPHICAPI_HPP
# define GRAPHICAPI_HPP

#include "GraphicPipeline.hpp"
#include "Buffer.hpp"
#include "FrameBuffer.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/Types.hpp"
#include "Math/Vector.hpp"

#ifdef GFX_BUILD_IMGUI
    #include "imgui/imgui.h"
#endif

namespace gfx
{

class GraphicAPI
{
public:
    GraphicAPI(const GraphicAPI&) = delete;
    GraphicAPI(GraphicAPI&&)      = delete;
    
    virtual utils::SharedPtr<Shader> newShader(const Shader::MetalShaderDescriptor&, const Shader::OpenGLShaderDescriptor&) const = 0;
    virtual utils::SharedPtr<GraphicPipeline> newGraphicsPipeline(const GraphicPipeline::Descriptor&) const = 0;
    virtual utils::SharedPtr<Buffer> newBuffer(const Buffer::Descriptor&) const = 0;
    virtual utils::SharedPtr<Texture> newTexture(const Texture::Descriptor&) const = 0;
    virtual utils::SharedPtr<FrameBuffer> newFrameBuffer(const utils::SharedPtr<Texture>& colorTexture) const = 0;

    #ifdef GFX_BUILD_IMGUI
        virtual void initImgui(ImGuiConfigFlags flags) = 0;
        inline void initImgui() { initImgui(ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable); }
    #endif

    virtual void beginFrame() = 0;

    virtual void setLoadAction(LoadAction) = 0;
    virtual void setClearColor(math::rgba) = 0;

    virtual void beginRenderPass() = 0;
    virtual void beginRenderPass(const utils::SharedPtr<FrameBuffer>&) = 0;
    #ifdef GFX_BUILD_IMGUI
        virtual void beginImguiRenderPass() = 0;
    #endif
    
    virtual void useGraphicsPipeline(const utils::SharedPtr<GraphicPipeline>&) = 0;
    virtual void useVertexBuffer(const utils::SharedPtr<Buffer>&) = 0;

    virtual void drawVertices(utils::uint32 start, utils::uint32 count) = 0;
    virtual void drawIndexedVertices(const utils::SharedPtr<Buffer>&) = 0;

    virtual void endRenderPass() = 0;

    virtual void endFrame() = 0;

    #ifdef GFX_BUILD_IMGUI
        virtual void terminateImGui() = 0;
    #endif

    virtual ~GraphicAPI() = default;

protected:
    GraphicAPI() = default;

public:
    GraphicAPI& operator = (const GraphicAPI&) = delete;
    GraphicAPI& operator = (GraphicAPI&&)      = delete;
};

}

#endif // GRAPHICAPI_HPP
