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
#include "UtilsCPP/SharedPtr.hpp"
#include "Graphics/Window.hpp"

#ifdef OBJCPP
    #import <Metal/Metal.h>
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
    friend utils::SharedPtr<GraphicAPI> GraphicAPI::newMetalGraphicAPI(const utils::SharedPtr<Window>& renderTarget);

public:
    MetalGraphicAPI(const MetalGraphicAPI&) = delete;
    MetalGraphicAPI(MetalGraphicAPI&&)      = delete;

    utils::SharedPtr<VertexBuffer> newVertexBuffer(void* data, utils::uint64 size, const VertexBuffer::LayoutBase& layout) override;
    utils::SharedPtr<GraphicPipeline> newGraphicsPipeline(const utils::String& vertexShaderName, const utils::String& fragmentShaderName) override;

    void setRenderTarget(const utils::SharedPtr<Window>&) override;

    void setClearColor(float r, float g, float b, float a) override;

    void beginFrame() override;

    void useGraphicsPipeline(utils::SharedPtr<GraphicPipeline>) override;
    void useVertexBuffer(utils::SharedPtr<VertexBuffer>) override;
    
    void drawVertices(utils::uint32 start, utils::uint32 count) override;
    
    void endFrame() override;

    ~MetalGraphicAPI() override;

private:
    MetalGraphicAPI(const utils::SharedPtr<Window>& renderTarget);

    utils::SharedPtr<Window> m_renderTarget;
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