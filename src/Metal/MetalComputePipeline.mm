#include "Metal/MetalComputePipeline.hpp"

#include "Metal/MetalDevice.hpp"
#include "Metal/MetalShaderFunction.hpp"

#include <cassert>

namespace gfx
{

MetalComputePipeline::MetalComputePipeline(const MetalDevice& device, const ComputePipeline::Descriptor& desc) { @autoreleasepool
{
    assert(desc.computeShader);
    assert(desc.threadsPerThreadgroupX > 0);
    assert(desc.threadsPerThreadgroupY > 0);
    assert(desc.threadsPerThreadgroupZ > 0);

    auto* computeFunction = dynamic_cast<MetalShaderFunction*>(desc.computeShader);
    assert(computeFunction);

    NSError* error = nil;
    m_computePipelineState = [device.mtlDevice() newComputePipelineStateWithFunction:computeFunction->mtlFunction() error:&error];
    if (m_computePipelineState == nil)
        throw std::runtime_error(error == nil ? "failed to create the ComputePipelineState" : [error.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding]);

    const NSUInteger threadCount = static_cast<NSUInteger>(desc.threadsPerThreadgroupX)
        * static_cast<NSUInteger>(desc.threadsPerThreadgroupY)
        * static_cast<NSUInteger>(desc.threadsPerThreadgroupZ);
    assert(threadCount <= m_computePipelineState.maxTotalThreadsPerThreadgroup);

    m_threadsPerThreadgroup = MTLSizeMake(
        desc.threadsPerThreadgroupX,
        desc.threadsPerThreadgroupY,
        desc.threadsPerThreadgroupZ);
}}

} // namespace gfx
