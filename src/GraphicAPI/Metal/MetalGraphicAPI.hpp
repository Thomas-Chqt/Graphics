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
    friend utils::SharedPtr<GraphicAPI> Platform::newMetalGraphicAPI(const utils::SharedPtr<Window>& renderTarget);

public:
    MetalGraphicAPI(const MetalGraphicAPI&) = delete;
    MetalGraphicAPI(MetalGraphicAPI&&)      = delete;

    void setRenderTarget(const utils::SharedPtr<Window>&) override;

#ifdef IMGUI_ENABLED
    void useForImGui() override;
#endif

    void setClearColor(float r, float g, float b, float a) override;

    utils::SharedPtr<VertexBuffer> newVertexBuffer(void* data, utils::uint64 size, const VertexBuffer::LayoutBase& layout) override;
    utils::SharedPtr<GraphicPipeline> newGraphicsPipeline(const utils::String& vertexShaderName, const utils::String& fragmentShaderName) override;
    utils::SharedPtr<IndexBuffer> newIndexBuffer(const utils::Array<utils::uint32>& indices) override;

    void beginFrame() override;

    void useGraphicsPipeline(utils::SharedPtr<GraphicPipeline>) override;
    void useVertexBuffer(utils::SharedPtr<VertexBuffer>) override;
    
    void drawVertices(utils::uint32 start, utils::uint32 count) override;
    void drawIndexedVertices(utils::SharedPtr<IndexBuffer>) override;
    
    void endFrame() override;

    ~MetalGraphicAPI() override;

private:
    MetalGraphicAPI(const utils::SharedPtr<Window>& renderTarget);

    utils::SharedPtr<MetalWindow> m_renderTarget;
    id<MTLDevice> m_mtlDevice = nullptr;
    id<MTLCommandQueue> m_commandQueue = nullptr;
    MTLRenderPassDescriptor* m_renderPassDescriptor = nullptr;
    NSAutoreleasePool* m_frameAutoreleasePool = nullptr;
    id<MTLCommandBuffer> m_commandBuffer = nullptr;
    id<CAMetalDrawable> m_currentDrawable = nullptr;
    id<MTLRenderCommandEncoder> m_commandEncoder = nullptr;
    id<MTLLibrary> m_shaderLibrary = nullptr;

public:
    MetalGraphicAPI& operator = (const MetalGraphicAPI&) = delete;
    MetalGraphicAPI& operator = (MetalGraphicAPI&&)      = delete;
};

}

#endif // METALGRAPHICAPI_HPP