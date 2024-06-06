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
#include "Graphics/Texture.hpp"
#include "IndexBuffer.hpp"
#include "Math/Matrix.hpp"
#ifdef GFX_IMGUI_ENABLED
    #include "UtilsCPP/Func.hpp"
#endif
#include "UtilsCPP/String.hpp"
#include "VertexBuffer.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Window.hpp"
#include "UtilsCPP/Types.hpp"
#include "Math/Vector.hpp"

namespace gfx
{

class GraphicAPI
{
public:
    GraphicAPI(const GraphicAPI&) = delete;
    GraphicAPI(GraphicAPI&&)      = delete;

    virtual void setRenderTarget(const utils::SharedPtr<Window>&) = 0;

#ifdef GFX_IMGUI_ENABLED
    virtual void useForImGui(const utils::Func<void()>& f = utils::Func<void()>()) = 0;
#endif

    virtual void setClearColor(const math::rgba& color) = 0;

    template<typename T>
    inline utils::SharedPtr<VertexBuffer> newVertexBuffer(const utils::Array<T>& vertices) const
    {
        return newVertexBuffer((void*)(const T*)vertices, vertices.length() * sizeof(T), VertexBuffer::Layout<T>());
    }

    virtual utils::SharedPtr<VertexBuffer> newVertexBuffer(void* data, utils::uint64 size, const VertexBuffer::LayoutBase& layout) const = 0;
    virtual utils::SharedPtr<GraphicPipeline> newGraphicsPipeline(const utils::String& vertexShaderName, const utils::String& fragmentShaderName, GraphicPipeline::BlendingOperation = GraphicPipeline::BlendingOperation::srcA_plus_1_minus_srcA) = 0;
    virtual utils::SharedPtr<IndexBuffer> newIndexBuffer(const utils::Array<utils::uint32>& indices) const = 0;
    virtual utils::SharedPtr<Texture> newTexture(utils::uint32 width, utils::uint32 height, Texture::PixelFormat = Texture::PixelFormat::RGBA) const = 0;

    virtual void beginFrame(bool clearBuffer = true) = 0;

    virtual void useGraphicsPipeline(const utils::SharedPtr<GraphicPipeline>&) = 0;
    virtual void useVertexBuffer(const utils::SharedPtr<VertexBuffer>&) = 0;
    
    virtual void setVertexUniform(utils::uint32 index, const math::vec4f&) = 0;
    virtual void setVertexUniform(utils::uint32 index, const math::mat4x4&) = 0;
    virtual void setVertexUniform(utils::uint32 index, const math::vec2f&) = 0;
    virtual void setVertexUniform(utils::uint32 index, const math::mat3x3&) = 0;
    
    virtual void setFragmentUniform(utils::uint32 index, const math::vec4f&) = 0;
    virtual void setFragmentTexture(utils::uint32 index, const utils::SharedPtr<Texture>&) = 0;

    virtual void drawVertices(utils::uint32 start, utils::uint32 count) = 0;
    virtual void drawIndexedVertices(const utils::SharedPtr<IndexBuffer>&) = 0;

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