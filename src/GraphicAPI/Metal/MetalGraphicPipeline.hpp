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
#include "UtilsCPP/Dictionary.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"

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
    friend utils::SharedPtr<GraphicPipeline> MetalGraphicAPI::newGraphicsPipeline(const utils::String&, const utils::String&, GraphicPipeline::BlendingOperation);
    
public:
    MetalGraphicPipeline()                            = delete;
    MetalGraphicPipeline(const MetalGraphicPipeline&) = delete;
    MetalGraphicPipeline(MetalGraphicPipeline&&)      = delete;

    inline utils::uint32 findVertexUniformIndex(const utils::String& name) override { return m_vertexUniformsIndices[name]; }
    inline utils::uint32 findFragmentUniformIndex(const utils::String& name) override { return m_fragmentUniformsIndices[name]; }

    inline id<MTLRenderPipelineState> renderPipelineState() { return m_renderPipelineState; }
    
    ~MetalGraphicPipeline();

private:
    MetalGraphicPipeline(id<MTLDevice>, id<MTLLibrary>, CAMetalLayer*, const utils::String& vertexShaderName, const utils::String& fragmentShaderName, GraphicPipeline::BlendingOperation);

    id<MTLRenderPipelineState> m_renderPipelineState = nullptr;

    utils::Dictionary<utils::String, utils::uint32> m_vertexUniformsIndices;
    utils::Dictionary<utils::String, utils::uint32> m_fragmentUniformsIndices;

public:
    MetalGraphicPipeline& operator = (const MetalGraphicPipeline&) = delete;
    MetalGraphicPipeline& operator = (MetalGraphicPipeline&&)      = delete;
};

}

#endif // METALGRAPHICPIPELINE_HPP