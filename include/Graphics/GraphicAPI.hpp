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
#include "Graphics/StructLayout.hpp"
#include "Graphics/Texture.hpp"
#include "IndexBuffer.hpp"
#include "Math/Matrix.hpp"
#include "UtilsCPP/String.hpp" // IWYU pragma: keep
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
    GraphicAPI(GraphicAPI&&)      = delete;

    template<typename T> inline utils::SharedPtr<VertexBuffer> newVertexBuffer(const utils::Array<T>& vertices) const { return newVertexBuffer((void*)(const T*)vertices, vertices.length(), sizeof(T), getLayout<T>()); }
    template<typename T> inline void setFragmentUniform(const utils::String& name, const T& data) { setFragmentUniform(name, (const void*)&data, sizeof(T), getLayout<T>()); }
    template<typename T> inline void setFragmentUniform(const utils::String& name, const utils::Array<T>& data) { setFragmentUniform(name, (const void*)data, data.length(), sizeof(T), getLayout<T>()); }

    #ifdef GFX_IMGUI_ENABLED
    virtual void useForImGui(ImGuiConfigFlags flags = 0) = 0;
    #endif

    #ifdef GFX_METAL_ENABLED
    virtual void initMetalShaderLib(const utils::String& path) = 0;
    #endif
    
    virtual utils::SharedPtr<VertexBuffer> newVertexBuffer(void* data, utils::uint64 count, utils::uint32 size, const StructLayout&) const = 0;
    virtual utils::SharedPtr<GraphicPipeline> newGraphicsPipeline(const GraphicPipeline::Descriptor&) const = 0;
    virtual utils::SharedPtr<IndexBuffer> newIndexBuffer(const utils::Array<utils::uint32>&) const = 0;
    virtual utils::SharedPtr<Texture> newTexture(const Texture::Descriptor&) const = 0;
    virtual utils::SharedPtr<FrameBuffer> newFrameBuffer(const utils::SharedPtr<Texture>& colorTexture = utils::SharedPtr<Texture>()) const = 0;

    virtual void beginFrame() = 0;

    virtual void setLoadAction(LoadAction) = 0;
    virtual void setClearColor(math::rgba) = 0;

    virtual void beginOnScreenRenderPass() = 0;
    virtual void beginOffScreenRenderPass(const utils::SharedPtr<FrameBuffer>&) = 0;

    virtual void useGraphicsPipeline(const utils::SharedPtr<GraphicPipeline>&) = 0;
    virtual void useVertexBuffer(const utils::SharedPtr<VertexBuffer>&) = 0;

    virtual void setVertexUniform(const utils::String& name, const math::vec4f&) = 0;
    virtual void setVertexUniform(const utils::String& name, const math::mat4x4&) = 0;
    virtual void setVertexUniform(const utils::String& name, const math::vec2f&) = 0;
    virtual void setVertexUniform(const utils::String& name, const math::mat3x3&) = 0;

    virtual void setFragmentUniform(const utils::String& name, utils::uint32) = 0;
    virtual void setFragmentUniform(const utils::String& name, float) = 0;
    virtual void setFragmentUniform(const utils::String& name, const math::vec3f&) = 0;
    virtual void setFragmentUniform(const utils::String& name, const math::vec4f&) = 0;
    virtual void setFragmentUniform(const utils::String& name, const void* data, utils::uint32 size, const StructLayout&) = 0;
    virtual void setFragmentUniform(const utils::String& name, const void* data, utils::uint32 len, utils::uint32 elementSize, const StructLayout&) = 0;

    virtual void setFragmentTexture(const utils::String& name, const utils::SharedPtr<Texture>&) = 0;

    virtual void drawVertices(utils::uint32 start, utils::uint32 count) = 0;
    virtual void drawIndexedVertices(const utils::SharedPtr<IndexBuffer>&) = 0;

    virtual void endOnScreenRenderPass() = 0;
    virtual void endOffScreeRenderPass() = 0;

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
