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

MetalSampler::MetalSampler(const id<MTLDevice>& device, const Sampler::Descriptor& descriptor) { @autoreleasepool
{
    MTLSamplerDescriptor* mtlSamplerDescriptor = [[[MTLSamplerDescriptor alloc] init] autorelease];

    mtlSamplerDescriptor.sAddressMode = (MTLSamplerAddressMode)toMTLSamplerAddressMode(descriptor.sAddressMode);
    mtlSamplerDescriptor.tAddressMode = (MTLSamplerAddressMode)toMTLSamplerAddressMode(descriptor.tAddressMode);
    mtlSamplerDescriptor.minFilter = (MTLSamplerMinMagFilter)toMTLSamplerMinMagFilter(descriptor.minFilter);
    mtlSamplerDescriptor.magFilter = (MTLSamplerMinMagFilter)toMTLSamplerMinMagFilter(descriptor.magFilter);
    mtlSamplerDescriptor.label = [[[NSString alloc] initWithCString:descriptor.debugName encoding:NSUTF8StringEncoding] autorelease];

    m_mtlSamplerState = [device newSamplerStateWithDescriptor:mtlSamplerDescriptor];
    if (m_mtlSamplerState == nil)
        throw MTLSamplerStateCreationError();
}}

}