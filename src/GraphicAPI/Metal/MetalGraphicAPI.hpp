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

#include "Graphics/GraphicAPI.hpp"
#include "Graphics/Platform.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Graphics/Window.hpp"
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
#endif // OBJCPP


namespace gfx
{

class MetalFrameBuffer;

class MetalGraphicAPI : public GraphicAPI
{
private:
    friend utils::SharedPtr<GraphicAPI> Platform::newMetalGraphicAPI(const utils::SharedPtr<Window>& renderTarget) const;

public:
    MetalGraphicAPI()                       = delete;
    MetalGraphicAPI(const MetalGraphicAPI&) = delete;
    MetalGraphicAPI(MetalGraphicAPI&&)      = delete;

#ifdef GFX_IMGUI_ENABLED
    void useForImGui(ImGuiConfigFlags flags = 0) override;
#endif

    utils::SharedPtr<VertexBuffer> newVertexBuffer(void* data, utils::uint64 size, const VertexBuffer::LayoutBase& layout) const override;
    utils::SharedPtr<GraphicPipeline> newGraphicsPipeline(const utils::String& vertexShaderName, const utils::String& fragmentShaderName) const override;
    utils::SharedPtr<IndexBuffer> newIndexBuffer(const utils::Array<utils::uint32>& indices) const override;
    utils::SharedPtr<Texture> newTexture(utils::uint32 width, utils::uint32 height, Texture::PixelFormat = Texture::PixelFormat::RGBA) const override;
    utils::SharedPtr<FrameBuffer> newFrameBuffer(utils::uint32 width, utils::uint32 height) const override;

    void beginFrame(const RenderPassDescriptor& = RenderPassDescriptor()) override;

    void useGraphicsPipeline(const utils::SharedPtr<GraphicPipeline>&) override;
    void useVertexBuffer(const utils::SharedPtr<VertexBuffer>&) override;

    void setVertexUniform(utils::uint32 index, const math::vec4f& vec) override;
    void setVertexUniform(utils::uint32 index, const math::mat4x4& mat) override;
    void setVertexUniform(utils::uint32 index, const math::vec2f&) override;
    void setVertexUniform(utils::uint32 index, const math::mat3x3& mat) override;

    void setFragmentUniform(utils::uint32 index, const math::vec4f& vec) override;
    void setFragmentTexture(utils::uint32 index, const utils::SharedPtr<Texture>&) override;
    void setFragmentTexture(utils::uint32 index, const utils::SharedPtr<FrameBuffer>&) override;

    void drawVertices(utils::uint32 start, utils::uint32 count) override;
    void drawIndexedVertices(const utils::SharedPtr<IndexBuffer>&) override;

    void nextRenderPass(const RenderPassDescriptor& = RenderPassDescriptor()) override;
    
    void endFrame() override;

    ~MetalGraphicAPI() override;

private:
    MetalGraphicAPI(const utils::SharedPtr<Window>& renderTarget);

    void beginRenderPass(const RenderPassDescriptor&);
    void endRenderPass();

    utils::SharedPtr<MetalWindow> m_window;

    // life time
    id<MTLDevice> m_mtlDevice = nullptr;
    id<MTLCommandQueue> m_commandQueue = nullptr;

    // frame time
    id<MTLCommandBuffer> m_commandBuffer = nullptr;
    id<CAMetalDrawable> m_currentDrawable = nullptr;

    // pass time
    utils::uint32 m_renderPassTargetPixelFormat; 
    id<MTLRenderCommandEncoder> m_commandEncoder = nullptr;
    utils::Array<utils::UniquePtr<utils::SharedPtrBase>> m_renderPassObjects;

public:
    MetalGraphicAPI& operator = (const MetalGraphicAPI&) = delete;
    MetalGraphicAPI& operator = (MetalGraphicAPI&&)      = delete;
};

}

#endif // METALGRAPHICAPI_HPP