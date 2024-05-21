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
#include "UtilsCPP/String.hpp"
#include "VertexBuffer.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Window.hpp"
#include "UtilsCPP/Types.hpp"

#ifdef USING_APPKIT
    #define newDefaultGraphicAPI(target) newMetalGraphicAPI(target)
#else
    #define newDefaultGraphicAPI(target) newOpenGLGraphicAPI(target)
#endif

namespace gfx
{

class GraphicAPI
{
public:
    GraphicAPI(const GraphicAPI&) = delete;
    GraphicAPI(GraphicAPI&&)      = delete;

#ifdef USING_OPENGL
    static utils::SharedPtr<GraphicAPI> newOpenGLGraphicAPI(const utils::SharedPtr<Window>& renderTarget = utils::SharedPtr<Window>());
#endif
#ifdef USING_METAL
    static utils::SharedPtr<GraphicAPI> newMetalGraphicAPI(const utils::SharedPtr<Window>& renderTarget = utils::SharedPtr<Window>());
#endif
    template<typename T>
    inline utils::SharedPtr<VertexBuffer> newVertexBuffer(const utils::Array<T>& vertices) { return newVertexBuffer((void*)(const T*)vertices, vertices.length() * sizeof(T), VertexBuffer::Layout<T>()); }
    virtual utils::SharedPtr<VertexBuffer> newVertexBuffer(void* data, utils::uint64 size, const VertexBuffer::LayoutBase& layout) = 0;
    virtual utils::SharedPtr<GraphicPipeline> newGraphicsPipeline(const utils::String& vertexShaderName, const utils::String& fragmentShaderName) = 0;
    
    virtual void setRenderTarget(const utils::SharedPtr<Window>&) = 0;

    virtual void setClearColor(float r, float g, float b, float a) = 0;    

    virtual void beginFrame() = 0;

    virtual void useGraphicsPipeline(utils::SharedPtr<GraphicPipeline>) = 0;
    virtual void useVertexBuffer(utils::SharedPtr<VertexBuffer>) = 0;

    virtual void drawVertices(utils::uint32 start, utils::uint32 count) = 0;

    virtual void endFrame() = 0;
    
    virtual ~GraphicAPI() = default;

protected:
    GraphicAPI() = default;


public:
    GraphicAPI& operator = (const GraphicAPI&) = delete;
    GraphicAPI& operator = (GraphicAPI&&)      = delete;
};

}

#endif // GRAPHICAPI_HPP