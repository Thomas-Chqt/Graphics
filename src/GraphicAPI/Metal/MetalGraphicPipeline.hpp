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
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
    #import <QuartzCore/CAMetalLayer.h>
#else
    template<typename T> using id = void*;

    class MTLDevice;
    class MTLLibrary;
    class MTLRenderPipelineState;
    class MTLDepthStencilState;
#endif // OBJCPP


namespace gfx
{

class MetalGraphicPipeline : public GraphicPipeline
{    
public:
    MetalGraphicPipeline()                            = delete;
    MetalGraphicPipeline(const MetalGraphicPipeline&) = delete;
    MetalGraphicPipeline(MetalGraphicPipeline&&)      = delete;

    MetalGraphicPipeline(id<MTLDevice>, id<MTLLibrary>, const GraphicPipeline::Descriptor&);

    inline utils::uint32 findVertexUniformIndex(const utils::String& name) override { return m_vertexUniformsIndices[name]; }
    inline utils::uint32 findFragmentUniformIndex(const utils::String& name) override { return m_fragmentUniformsIndices[name]; }
    
    const id<MTLRenderPipelineState>& renderPipelineState() { return m_renderPipelineState; };
    const id<MTLDepthStencilState>& depthStencilState() { return m_depthStencilState; };
    
    ~MetalGraphicPipeline();

private:
    id<MTLRenderPipelineState> m_renderPipelineState = nullptr;
    id<MTLDepthStencilState> m_depthStencilState = nullptr;

    utils::Dictionary<utils::String, utils::uint32> m_vertexUniformsIndices;
    utils::Dictionary<utils::String, utils::uint32> m_fragmentUniformsIndices;

public:
    MetalGraphicPipeline& operator = (const MetalGraphicPipeline&) = delete;
    MetalGraphicPipeline& operator = (MetalGraphicPipeline&&)      = delete;
};

}

#endif // METALGRAPHICPIPELINE_HPP