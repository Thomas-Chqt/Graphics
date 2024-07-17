/*
 * ---------------------------------------------------
 * MetalSampler.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/12 13:10:35
 * ---------------------------------------------------
 */

#include "GraphicAPI/Metal/MetalSampler.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Error.hpp"
#include "Graphics/Sampler.hpp"
#include <Metal/Metal.h>

namespace gfx
{

MTLSamplerAddressMode toMTLSamplerAddressMode(Sampler::AddressMode addressMode)
{
    switch (addressMode)
    {
    case Sampler::AddressMode::ClampToEdge:
        return MTLSamplerAddressModeClampToEdge;
        break;
    case Sampler::AddressMode::Repeat:
        return MTLSamplerAddressModeRepeat;
        break;
    case Sampler::AddressMode::MirrorRepeat:
        return MTLSamplerAddressModeMirrorRepeat;
        break;
    }
}

MTLSamplerMinMagFilter toMTLSamplerMinMagFilter(Sampler::MinMagFilter filter)
{
    switch (filter)
    {
    case Sampler::MinMagFilter::Nearest:
        return MTLSamplerMinMagFilterNearest;
        break;
    case Sampler::MinMagFilter::Linear:
        return MTLSamplerMinMagFilterLinear;
        break;
    }
}

MetalSampler::MetalSampler(const id<MTLDevice>& device, const Sampler::Descriptor& descriptor) { @autoreleasepool
{
    MTLSamplerDescriptor* mtlSamplerDescriptor = [[[MTLSamplerDescriptor alloc] init] autorelease];

    mtlSamplerDescriptor.sAddressMode = toMTLSamplerAddressMode(descriptor.sAddressMode);
    mtlSamplerDescriptor.tAddressMode = toMTLSamplerAddressMode(descriptor.tAddressMode);
    mtlSamplerDescriptor.minFilter = toMTLSamplerMinMagFilter(descriptor.minFilter);
    mtlSamplerDescriptor.magFilter = toMTLSamplerMinMagFilter(descriptor.magFilter);
    mtlSamplerDescriptor.label = [[[NSString alloc] initWithCString:descriptor.debugName encoding:NSUTF8StringEncoding] autorelease];

    m_mtlSamplerState = [device newSamplerStateWithDescriptor:mtlSamplerDescriptor];
    if (m_mtlSamplerState == nil)
        throw MTLSamplerStateCreationError();
}}

}