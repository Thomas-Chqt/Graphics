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

#include "Graphics/GraphicPipeline.hpp"
#include "UtilsCPP/Dictionary.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
    #import <QuartzCore/CAMetalLayer.h>
#else
    template<typename T> using id = T*;

    class MTLDevice;
    class MTLRenderPipelineState;
    class MTLDepthStencilState;
#endif // __OBJC__


namespace gfx
{

class MetalGraphicPipeline : public GraphicPipeline
{    
public:
    MetalGraphicPipeline()                            = delete;
    MetalGraphicPipeline(const MetalGraphicPipeline&) = delete;
    MetalGraphicPipeline(MetalGraphicPipeline&&)      = delete;

    MetalGraphicPipeline(const id<MTLDevice>&, const GraphicPipeline::Descriptor&);

    inline utils::uint64 getVertexBufferIndex(const utils::String& name) override { return m_vertexBufferBindingsIndices[name]; }
    inline utils::uint64 getVertexTextureIndex(const utils::String& name) override { return m_vertexTextureBindingsIndices[name]; }
    inline utils::uint64 getVertexSamplerIndex(const utils::String& name) override { return m_vertexSamplerBindingsIndices[name]; }

    inline utils::uint64 getFragmentBufferIndex(const utils::String& name) override { return m_fragmentBufferBindingsIndices[name]; }
    inline utils::uint64 getFragmentTextureIndex(const utils::String& name) override { return m_fragmentTextureBindingsIndices[name]; }
    inline utils::uint64 getFragmentSamplerIndex(const utils::String& name) override { return m_fragmentSamplerBindingsIndices[name]; }

    const id<MTLRenderPipelineState>& renderPipelineState() { return m_renderPipelineState; };
    const id<MTLDepthStencilState>& depthStencilState() { return m_depthStencilState; };
    
    ~MetalGraphicPipeline() override;

private:
    id<MTLRenderPipelineState> m_renderPipelineState = nullptr;
    id<MTLDepthStencilState> m_depthStencilState = nullptr;

    utils::Dictionary<utils::String, utils::uint64> m_vertexBufferBindingsIndices;
    utils::Dictionary<utils::String, utils::uint64> m_vertexTextureBindingsIndices;
    utils::Dictionary<utils::String, utils::uint64> m_vertexSamplerBindingsIndices;

    utils::Dictionary<utils::String, utils::uint64> m_fragmentBufferBindingsIndices;
    utils::Dictionary<utils::String, utils::uint64> m_fragmentTextureBindingsIndices;
    utils::Dictionary<utils::String, utils::uint64> m_fragmentSamplerBindingsIndices;

public:
    MetalGraphicPipeline& operator = (const MetalGraphicPipeline&) = delete;
    MetalGraphicPipeline& operator = (MetalGraphicPipeline&&)      = delete;
};

}

#endif // METALGRAPHICPIPELINE_HPP
