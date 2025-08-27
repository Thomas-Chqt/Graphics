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

    inline const id<MTLRenderPipelineState>& renderPipelineState() const { return m_renderPipelineState; }
    inline const id<MTLDepthStencilState>& depthStencilState() const { return m_depthStencilState; }

    ~MetalGraphicsPipeline() override;

private:
    id<MTLRenderPipelineState> m_renderPipelineState = nil;
    id<MTLDepthStencilState> m_depthStencilState = nil;

public:
    MetalGraphicsPipeline& operator=(const MetalGraphicsPipeline&) = delete;
    MetalGraphicsPipeline& operator=(MetalGraphicsPipeline&&) = delete;
};

} // namespace gfx

#endif // METALGRAPHICSPIPELINE_HPP
