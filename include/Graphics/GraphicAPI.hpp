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
#include "Graphics/FrameBuffer.hpp"
#include "Graphics/Texture.hpp"
#include "IndexBuffer.hpp"
#include "Math/Matrix.hpp"
#include "VertexBuffer.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/Types.hpp"
#include "Math/Vector.hpp"

#ifdef GFX_IMGUI_ENABLED
    #include "imgui/imgui.h"
#endif

namespace gfx
{

class GraphicAPI
{
public:
    GraphicAPI(const GraphicAPI&) = delete;
    GraphicAPI(GraphicAPI&&) = delete;

    #ifdef GFX_IMGUI_ENABLED
    virtual void useForImGui(ImGuiConfigFlags flags = 0) = 0;
    #endif

    template<typename T>
    inline utils::SharedPtr<VertexBuffer> newVertexBuffer(const utils::Array<T>& vertices) const
    {
        return newVertexBuffer((const T*)vertices, vertices.length(), sizeof(T), VertexBuffer::getLayout<T>());
    }

    virtual utils::SharedPtr<VertexBuffer> newVertexBuffer(void* data, utils::uint64 count, utils::uint32 vertexSize, const utils::Array<VertexBuffer::LayoutElement>& layout) const = 0;
    virtual utils::SharedPtr<GraphicPipeline> newGraphicsPipeline(const GraphicPipeline::Descriptor&) const = 0;
    virtual utils::SharedPtr<IndexBuffer> newIndexBuffer(const utils::Array<utils::uint32>&) const = 0;
    virtual utils::SharedPtr<Texture> newTexture(const Texture::Descriptor&) const = 0;
    virtual utils::SharedPtr<FrameBuffer> newFrameBuffer(const FrameBuffer::Descriptor&) const = 0;

    virtual void setLoadAction(LoadAction);
    virtual void setClearColor(math::rgba);
    virtual void setRenderTarget(const utils::SharedPtr<FrameBuffer>&);

    virtual void beginFrame() = 0;

    virtual void useGraphicsPipeline(const utils::SharedPtr<GraphicPipeline>&) = 0;
    virtual void useVertexBuffer(const utils::SharedPtr<VertexBuffer>&) = 0;

    virtual void setVertexUniform(utils::uint32 index, const math::vec4f&) = 0;
    virtual void setVertexUniform(utils::uint32 index, const math::mat4x4&) = 0;
    virtual void setVertexUniform(utils::uint32 index, const math::vec2f&) = 0;
    virtual void setVertexUniform(utils::uint32 index, const math::mat3x3&) = 0;

    virtual void setFragmentUniform(utils::uint32 index, const math::vec4f&) = 0;
    virtual void setFragmentTexture(utils::uint32 index, const utils::SharedPtr<Texture>&) = 0;
    virtual void setFragmentTexture(utils::uint32 index, const utils::SharedPtr<FrameBuffer>&) = 0;

    virtual void drawVertices(utils::uint32 start, utils::uint32 count) = 0;
    virtual void drawIndexedVertices(const utils::SharedPtr<IndexBuffer>&) = 0;

    virtual void nextRenderPass() = 0;

    virtual void endFrame() = 0;

    virtual ~GraphicAPI() = default;

protected:
    GraphicAPI() = default;


    #ifdef GFX_IMGUI_ENABLED
    static GraphicAPI* s_imguiEnabledAPI;
    #endif

public:
    GraphicAPI& operator = (const GraphicAPI&) = delete;
    GraphicAPI& operator = (GraphicAPI&&)      = delete;
};
}

#endif // GRAPHICAPI_HPP
