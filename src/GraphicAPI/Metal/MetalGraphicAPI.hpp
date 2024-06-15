/*
 * ---------------------------------------------------
 * MetalGraphicAPI.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/19 20:30:42
 * ---------------------------------------------------
 */

#ifndef METALGRAPHICAPI_HPP
# define METALGRAPHICAPI_HPP

#include "GraphicAPI/Metal/MetalTexture.hpp"
#include "Graphics/FrameBuffer.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/Window.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/Types.hpp"
#include "Window/MetalWindow.hpp"
#ifdef GFX_IMGUI_ENABLED
    #include "imgui/imgui.h"
#endif

#ifdef __OBJC__
    #import <Metal/Metal.h>
    #import <QuartzCore/CAMetalLayer.h>
#else
    template<typename T> using id = void*;

    class MTLDevice;
    class MTLCommandQueue;
    class MTLLibrary;
    class NSAutoreleasePool;
    class MTLCommandBuffer;
    class CAMetalDrawable;
    class MTLRenderCommandEncoder;
    class MTLRenderPassDescriptor;
    class MTLTexture;
#endif // __OBJC__

namespace gfx
{

class MetalFrameBuffer;

class MetalGraphicAPI : public GraphicAPI
{
public:
    MetalGraphicAPI()                       = delete;
    MetalGraphicAPI(const MetalGraphicAPI&) = delete;
    MetalGraphicAPI(MetalGraphicAPI&&)      = delete;

    MetalGraphicAPI(const utils::SharedPtr<Window>&);

    #ifdef GFX_IMGUI_ENABLED
    void useForImGui(ImGuiConfigFlags flags = 0) override;
    #endif

    #ifdef GFX_METAL_ENABLED
    void initMetalShaderLib(const utils::String& path) override;
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

    void setFragmentUniform(utils::uint32 index, float f) override;
    void setFragmentUniform(utils::uint32 index, const math::vec4f&) override;
    void setFragmentTexture(utils::uint32 index, const utils::SharedPtr<Texture>&) override;

    void drawVertices(utils::uint32 start, utils::uint32 count) override;
    void drawIndexedVertices(const utils::SharedPtr<IndexBuffer>&) override;

    void endOnScreenRenderPass() override;
    void endOffScreeRenderPass() override;

    void endFrame() override;

    ~MetalGraphicAPI() override;

private:
    // life time
    utils::SharedPtr<MetalWindow> m_window;
    id<MTLDevice> m_mtlDevice = nullptr;
    id<MTLCommandQueue> m_commandQueue = nullptr;
    id<MTLLibrary> m_shaderLib = nullptr;
    MetalTexture m_depthTexture;

    // frame time
    id<MTLCommandBuffer> m_commandBuffer = nullptr;
    id<CAMetalDrawable> m_currentDrawable = nullptr;

    // pass time
    LoadAction m_nextPassLoadAction = LoadAction::clear; 
    math::rgba m_nextPassClearColor = BLACK;
    id<MTLRenderCommandEncoder> m_commandEncoder = nullptr;
    utils::Array<utils::UniquePtr<utils::SharedPtrBase>> m_renderPassObjects;

public:
    MetalGraphicAPI& operator = (const MetalGraphicAPI&) = delete;
    MetalGraphicAPI& operator = (MetalGraphicAPI&&)      = delete;
};

}

#endif // METALGRAPHICAPI_HPP
