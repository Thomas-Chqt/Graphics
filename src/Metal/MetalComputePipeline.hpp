#ifndef METALCOMPUTEPIPELINE_HPP
#define METALCOMPUTEPIPELINE_HPP

#include "Graphics/ComputePipeline.hpp"

#if !defined(__OBJC__)
#error this file can only be used in objective c
#endif

namespace gfx
{

class MetalDevice;

class MetalComputePipeline final : public ComputePipeline
{
public:
    MetalComputePipeline() = delete;
    MetalComputePipeline(const MetalComputePipeline&) = delete;
    MetalComputePipeline(MetalComputePipeline&&) = delete;

    MetalComputePipeline(const MetalDevice&, const ComputePipeline::Descriptor&);

    id<MTLComputePipelineState> computePipelineState() const { return m_computePipelineState; }
    MTLSize threadsPerThreadgroup() const { return m_threadsPerThreadgroup; }

    ~MetalComputePipeline() override = default;

private:
    id<MTLComputePipelineState> m_computePipelineState = nil;
    MTLSize m_threadsPerThreadgroup = MTLSizeMake(1, 1, 1);

public:
    MetalComputePipeline& operator=(const MetalComputePipeline&) = delete;
    MetalComputePipeline& operator=(MetalComputePipeline&&) = delete;
};

} // namespace gfx

#endif // METALCOMPUTEPIPELINE_HPP
