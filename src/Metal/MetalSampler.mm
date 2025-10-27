/*
 * ---------------------------------------------------
 * MetalSampler.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/12 13:10:35
 * ---------------------------------------------------
 */

#include "Graphics/Sampler.hpp"

#include "Metal/MetalSampler.hpp"
#include "Metal/MetalDevice.hpp"

#import "Metal/MetalEnums.h"

namespace gfx
{

MetalSampler::MetalSampler(MetalSampler&& other) noexcept
    : m_mtlSamplerState(std::exchange(other.m_mtlSamplerState, nil))
{
}

MetalSampler::MetalSampler(const MetalDevice& device, const Sampler::Descriptor& descriptor) { @autoreleasepool
{
    MTLSamplerDescriptor* mtlSamplerDescriptor = [[[MTLSamplerDescriptor alloc] init] autorelease];

    mtlSamplerDescriptor.sAddressMode = toMTLSamplerAddressMode(descriptor.sAddressMode);
    mtlSamplerDescriptor.tAddressMode = toMTLSamplerAddressMode(descriptor.tAddressMode);
    mtlSamplerDescriptor.rAddressMode = toMTLSamplerAddressMode(descriptor.rAddressMode);
    mtlSamplerDescriptor.minFilter = toMTLSamplerMinMagFilter(descriptor.minFilter);
    mtlSamplerDescriptor.magFilter = toMTLSamplerMinMagFilter(descriptor.magFilter);
    mtlSamplerDescriptor.supportArgumentBuffers = YES;

    m_mtlSamplerState = [device.mtlDevice() newSamplerStateWithDescriptor:mtlSamplerDescriptor];
    if (m_mtlSamplerState == nil)
        throw std::runtime_error("sampler state creation failed");
}}

MetalSampler::~MetalSampler()
{
    if(m_mtlSamplerState != nil)
        [m_mtlSamplerState release];
}

MetalSampler& MetalSampler::operator=(MetalSampler&& other) noexcept
{
    if (this != &other)
    {
        if(m_mtlSamplerState != nil)
            [m_mtlSamplerState release];
        m_mtlSamplerState = std::exchange(other.m_mtlSamplerState, nil);
    }
    return *this;
}

} // namespace gfx
