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
#include "GraphicAPI/Metal/MetalSampler.hpp"
#include "GraphicAPI/Metal/MetalTexture.hpp"
#include "Graphics/FrameBuffer.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/Shader.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/Dictionary.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/Types.hpp"
#include "Window/MetalWindow.hpp"
#ifdef GFX_BUILD_IMGUI
    #include <imgui.h>
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
    
    explicit MetalGraphicAPI(const utils::SharedPtr<Window>&);

    inline id<MTLDevice> device() { return m_mtlDevice; }

    utils::SharedPtr<Shader> newShader(const Shader::Descriptor&) const override;
    utils::SharedPtr<GraphicPipeline> newGraphicsPipeline(const GraphicPipeline::Descriptor&) const override;
    utils::SharedPtr<Buffer> newBuffer(const Buffer::Descriptor&) const override;
    utils::SharedPtr<Texture> newTexture(const Texture::Descriptor&) const override;
    utils::SharedPtr<Sampler> newSampler(const Sampler::Descriptor&) const override;
    utils::SharedPtr<FrameBuffer> newFrameBuffer(const FrameBuffer::Descriptor&) const override;

    #ifdef GFX_BUILD_IMGUI
        void initImgui(ImGuiConfigFlags flags) override;
    #endif

    void beginFrame() override;

    inline void setLoadAction(LoadAction act) override { m_nextPassLoadAction = act; }
    inline void setClearColor(math::rgba col) override { m_nextPassClearColor = col; }

    void beginRenderPass() override;
    void beginRenderPass(const utils::SharedPtr<FrameBuffer>&) override;
    void beginRenderPass(const utils::SharedPtr<RenderTarget>&) override;
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

    ~MetalGraphicAPI() override;

private:
    // life time
    utils::SharedPtr<MetalWindow> m_window;
    id<MTLDevice> m_mtlDevice = nullptr;
    id<MTLCommandQueue> m_commandQueue = nullptr;

    // frame time
    id<MTLCommandBuffer> m_commandBuffer = nullptr;
    #ifdef GFX_BUILD_IMGUI
        bool m_isImguiFrame = false;
    #endif

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
    utils::Dictionary<utils::uint64, utils::SharedPtr<MetalBuffer>> m_vertexBuffers;
    utils::Dictionary<utils::uint64, utils::SharedPtr<MetalBuffer>> m_fragmentBuffers;
    utils::Dictionary<utils::uint64, utils::SharedPtr<MetalTexture>> m_fragmentTextures;
    utils::Dictionary<utils::uint64, utils::SharedPtr<MetalSampler>> m_fragmentSamplers;
    utils::SharedPtr<MetalBuffer> m_indexBuffer;

public:
    MetalGraphicAPI& operator = (const MetalGraphicAPI&) = delete;
    MetalGraphicAPI& operator = (MetalGraphicAPI&&)      = delete;
};

}

#endif // METALGRAPHICAPI_HPP
