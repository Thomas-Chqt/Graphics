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

#include "GraphicAPI/Metal/MetalBuffer.hpp"
#include "GraphicAPI/Metal/MetalGraphicPipeline.hpp"
#include "GraphicAPI/Metal/MetalTexture.hpp"
#include "Graphics/FrameBuffer.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/Shader.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/Types.hpp"
#include "Window/MetalWindow.hpp"
#ifdef GFX_BUILD_IMGUI
    #include "imgui/imgui.h"
#endif

#ifdef __OBJC__
    #import <Metal/Metal.h>
    #import <QuartzCore/CAMetalLayer.h>
#else
    template<typename T> using id = T*;

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

    inline id<MTLDevice> device() { return m_mtlDevice; }

    utils::SharedPtr<Shader> newShader(const Shader::MetalShaderDescriptor&, const Shader::OpenGLShaderDescriptor&) const override;
    utils::SharedPtr<GraphicPipeline> newGraphicsPipeline(const GraphicPipeline::Descriptor&) const override;
    utils::SharedPtr<Buffer> newBuffer(const Buffer::Descriptor&) const override;
    utils::SharedPtr<Texture> newTexture(const Texture::Descriptor&) const override;
    utils::SharedPtr<FrameBuffer> newFrameBuffer(const utils::SharedPtr<Texture>& colorTexture = utils::SharedPtr<Texture>()) const override;

    #ifdef GFX_BUILD_IMGUI
        void initImGui(ImGuiConfigFlags flags = ImGuiConfigFlags_DockingEnable) override;
    #endif

    void beginFrame() override;

    inline void setLoadAction(LoadAction act) override { m_nextPassLoadAction = act; }
    inline void setClearColor(math::rgba col) override { m_nextPassClearColor = col; }

    void beginRenderPass() override;
    void beginRenderPass(const utils::SharedPtr<FrameBuffer>&) override;
    #ifdef GFX_BUILD_IMGUI
        void beginImguiRenderPass() override;
    #endif

    void useGraphicsPipeline(const utils::SharedPtr<GraphicPipeline>&) override;
    void useVertexBuffer(const utils::SharedPtr<Buffer>&) override;

    void drawVertices(utils::uint32 start, utils::uint32 count) override;
    void drawIndexedVertices(const utils::SharedPtr<Buffer>&) override;

    void endRenderPass() override;

    void endFrame() override;
    
    #ifdef GFX_BUILD_IMGUI
        void terminateImGui() override;
    #endif

    ~MetalGraphicAPI() override;

private:
    // life time
    utils::SharedPtr<MetalWindow> m_window;
    id<MTLDevice> m_mtlDevice = nullptr;
    id<MTLCommandQueue> m_commandQueue = nullptr;
    #ifdef GFX_BUILD_IMGUI
        bool m_isImguiInit = false;
    #endif

    // frame time
    id<MTLCommandBuffer> m_commandBuffer = nullptr;

    // pass description
    LoadAction m_nextPassLoadAction = LoadAction::clear; 
    math::rgba m_nextPassClearColor = BLACK;
    
    // pass time
    #ifdef GFX_BUILD_IMGUI
        bool m_isImguiRenderPass = false;
    #endif
    id<MTLRenderCommandEncoder> m_commandEncoder = nullptr;
    utils::SharedPtr<MetalFrameBuffer> m_frameBuffer;
    utils::SharedPtr<MetalGraphicPipeline> m_graphicPipeline;
    utils::SharedPtr<MetalBuffer> m_vertexBuffer;
    utils::SharedPtr<MetalBuffer> m_indexBuffer;

public:
    MetalGraphicAPI& operator = (const MetalGraphicAPI&) = delete;
    MetalGraphicAPI& operator = (MetalGraphicAPI&&)      = delete;
};

}

#endif // METALGRAPHICAPI_HPP
