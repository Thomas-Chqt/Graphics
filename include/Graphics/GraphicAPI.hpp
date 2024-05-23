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
#include "IndexBuffer.hpp"
#include "UtilsCPP/String.hpp"
#include "VertexBuffer.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Window.hpp"
#include "UtilsCPP/Types.hpp"

namespace gfx
{

class GraphicAPI
{
public:
    GraphicAPI(const GraphicAPI&) = delete;
    GraphicAPI(GraphicAPI&&)      = delete;

    virtual void setRenderTarget(const utils::SharedPtr<Window>&) = 0;

#ifdef IMGUI_ENABLED
    virtual void useForImGui() = 0;
#endif

    virtual void setClearColor(float r, float g, float b, float a) = 0;

    virtual utils::SharedPtr<VertexBuffer> newVertexBuffer(void* data, utils::uint64 size, const VertexBuffer::LayoutBase& layout) = 0;
    template<typename T> inline utils::SharedPtr<VertexBuffer> newVertexBuffer(const utils::Array<T>& vertices) { return newVertexBuffer((void*)(const T*)vertices, vertices.length() * sizeof(T), VertexBuffer::Layout<T>()); }
    virtual utils::SharedPtr<GraphicPipeline> newGraphicsPipeline(const utils::String& vertexShaderName, const utils::String& fragmentShaderName) = 0;
    virtual utils::SharedPtr<IndexBuffer> newIndexBuffer(const utils::Array<utils::uint32>& indices) = 0;

    virtual void beginFrame() = 0;

    virtual void useGraphicsPipeline(utils::SharedPtr<GraphicPipeline>) = 0;
    virtual void useVertexBuffer(utils::SharedPtr<VertexBuffer>) = 0;

    virtual void drawVertices(utils::uint32 start, utils::uint32 count) = 0;
    virtual void drawIndexedVertices(utils::SharedPtr<IndexBuffer>) = 0;

    virtual void endFrame() = 0;
    
    virtual ~GraphicAPI() = default;

protected:
    GraphicAPI() = default;

#ifdef IMGUI_ENABLED
    inline static GraphicAPI* s_imguiEnabledAPI = nullptr;
#endif

public:
    GraphicAPI& operator = (const GraphicAPI&) = delete;
    GraphicAPI& operator = (GraphicAPI&&)      = delete;
};

}

#endif // GRAPHICAPI_HPP