/*
 * ---------------------------------------------------
 * MetalGraphicsPipeline.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/27 06:33:00
 * ---------------------------------------------------
 */

#ifndef METALGRAPHICSPIPELINE_HPP
#define METALGRAPHICSPIPELINE_HPP

#include "Graphics/GraphicsPipeline.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
template<typename T>
    using id = T*;
    #define nil nullptr

    class MTLDevice;
    class MTLRenderPipelineState;
    class MTLDepthStencilState;
#endif // __OBJC__

namespace gfx
{

class MetalDevice;

class MetalGraphicsPipeline : public GraphicsPipeline
{
public:
    MetalGraphicsPipeline() = delete;
    MetalGraphicsPipeline(const MetalGraphicsPipeline&) = delete;
    MetalGraphicsPipeline(MetalGraphicsPipeline&&) = delete;

    MetalGraphicsPipeline(const MetalDevice&, const GraphicsPipeline::Descriptor&);

    id<MTLRenderPipelineState> renderPipelineState(void) { return m_renderPipelineState; }
    id<MTLDepthStencilState> depthStencilState(void) { return m_depthStencilState; }

    ~MetalGraphicsPipeline();

private:
    id<MTLRenderPipelineState> m_renderPipelineState = nil;
    id<MTLDepthStencilState> m_depthStencilState = nil;

public:
    MetalGraphicsPipeline& operator=(const MetalGraphicsPipeline&) = delete;
    MetalGraphicsPipeline& operator=(MetalGraphicsPipeline&&) = delete;
};

} // namespace gfx

#endif // METALGRAPHICSPIPELINE_HPP
