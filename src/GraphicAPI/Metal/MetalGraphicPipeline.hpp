/*
 * ---------------------------------------------------
 * MetalGraphicPipeline.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/21 17:22:31
 * ---------------------------------------------------
 */

#ifndef METALGRAPHICPIPELINE_HPP
# define METALGRAPHICPIPELINE_HPP

#include "GraphicAPI/Metal/MetalGraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
    #import <QuartzCore/CAMetalLayer.h>
#else
    template<typename T> using id = void*;

    class MTLDevice;
    class MTLLibrary;
    class CAMetalLayer;
    class MTLRenderPipelineState;
#endif // OBJCPP


namespace gfx
{

class MetalGraphicPipeline : public GraphicPipeline
{
private:
    friend utils::SharedPtr<GraphicPipeline> MetalGraphicAPI::newGraphicsPipeline(const utils::String& vertexShaderName, const utils::String& fragmentShaderName);
    
public:
    MetalGraphicPipeline()                             = delete;
    MetalGraphicPipeline(const MetalGraphicPipeline&) = delete;
    MetalGraphicPipeline(MetalGraphicPipeline&&)      = delete;

    inline id<MTLRenderPipelineState> renderPipelineState() { return m_renderPipelineState; }
    
    ~MetalGraphicPipeline();

private:
    MetalGraphicPipeline(id<MTLDevice>, id<MTLLibrary>, CAMetalLayer*, const utils::String& vertexShaderName, const utils::String& fragmentShaderName);

    id<MTLRenderPipelineState> m_renderPipelineState = nullptr;

public:
    MetalGraphicPipeline& operator = (const MetalGraphicPipeline&) = delete;
    MetalGraphicPipeline& operator = (MetalGraphicPipeline&&)      = delete;
};

}

#endif // METALGRAPHICPIPELINE_HPP