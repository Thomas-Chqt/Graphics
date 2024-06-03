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
#include "Window/MetalWindow.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
    #import <QuartzCore/CAMetalLayer.h>
#else
    template<typename T> using id = void*;

    class MTLDevice;
    class MTLCommandQueue;
    class MTLRenderPassDescriptor;
    class MTLLibrary;
    class NSAutoreleasePool;
    class MTLCommandBuffer;
    class CAMetalDrawable;
    class MTLRenderCommandEncoder;
#endif // OBJCPP


namespace gfx
{

class MetalGraphicAPI : public GraphicAPI
{
private:
    friend utils::SharedPtr<GraphicAPI> Platform::newMetalGraphicAPI(const utils::SharedPtr<Window>& renderTarget) const;

public:
    MetalGraphicAPI()                       = delete;
    MetalGraphicAPI(const MetalGraphicAPI&) = delete;
    MetalGraphicAPI(MetalGraphicAPI&&)      = delete;

    void setRenderTarget(const utils::SharedPtr<Window>&) override;

#ifdef IMGUI_ENABLED
    void useForImGui(const utils::Func<void()>& f = utils::Func<void()>()) override;
#endif

    void setClearColor(const math::rgba& color) override;

    utils::SharedPtr<VertexBuffer> newVertexBuffer(void* data, utils::uint64 size, const VertexBuffer::LayoutBase& layout) const override;
    utils::SharedPtr<GraphicPipeline> newGraphicsPipeline(const utils::String& vertexShaderName, const utils::String& fragmentShaderName, GraphicPipeline::BlendingOperation = GraphicPipeline::BlendingOperation::srcA_plus_1_minus_srcA) override;
    utils::SharedPtr<IndexBuffer> newIndexBuffer(const utils::Array<utils::uint32>& indices) const override;
    utils::SharedPtr<Texture> newTexture(utils::uint32 width, utils::uint32 height, Texture::PixelFormat = Texture::PixelFormat::RGBA) const override;

    void beginFrame(bool clearBuffer = true) override;

    void useGraphicsPipeline(const utils::SharedPtr<GraphicPipeline>&) override;
    void useVertexBuffer(const utils::SharedPtr<VertexBuffer>&) override;

    void setVertexUniform(utils::uint32 index, const math::vec4f& vec) override;
    void setVertexUniform(utils::uint32 index, const math::mat4x4& mat) override;
    void setVertexUniform(utils::uint32 index, const math::vec2f&) override;
    void setVertexUniform(utils::uint32 index, const math::mat3x3& mat) override;

    void setFragmentUniform(utils::uint32 index, const math::vec4f& vec) override;
    void setFragmentTexture(utils::uint32 index, const utils::SharedPtr<Texture>&) override;

    void drawVertices(utils::uint32 start, utils::uint32 count) override;
    void drawIndexedVertices(const utils::SharedPtr<IndexBuffer>&) override;
    
    void endFrame() override;

    ~MetalGraphicAPI() override;

private:
    MetalGraphicAPI(const utils::SharedPtr<Window>& renderTarget);

    utils::SharedPtr<MetalWindow> m_renderTarget;

    id<MTLDevice> m_mtlDevice = nullptr;
    id<MTLCommandQueue> m_commandQueue = nullptr;
    MTLRenderPassDescriptor* m_renderPassDescriptor = nullptr;

    id<MTLLibrary> m_shaderLibrary = nullptr;
    
    id<MTLCommandBuffer> m_commandBuffer = nullptr;
    id<CAMetalDrawable> m_currentDrawable = nullptr;
    id<MTLRenderCommandEncoder> m_commandEncoder = nullptr;
    utils::Array<utils::UniquePtr<utils::SharedPtrBase>> m_frameObjects;
    int m_frameCount = 0;

public:
    MetalGraphicAPI& operator = (const MetalGraphicAPI&) = delete;
    MetalGraphicAPI& operator = (MetalGraphicAPI&&)      = delete;
};

}

#endif // METALGRAPHICAPI_HPP