/*
 * ---------------------------------------------------
 * MetalSampler.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/12 13:08:10
 * ---------------------------------------------------
 */

#ifndef METALSAMPLER_HPP
# define METALSAMPLER_HPP

#include "Graphics/Sampler.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    template<typename T> using id = T*;

    class MTLDevice;
    class MTLSamplerState;
#endif // __OBJC__


namespace gfx
{

class MetalSampler : public Sampler
{
public:
    MetalSampler()                    = delete;
    MetalSampler(const MetalSampler&) = delete;
    MetalSampler(MetalSampler&&)      = delete;
    
    MetalSampler(const id<MTLDevice>&, const Sampler::Descriptor&);

    inline const id<MTLSamplerState>& mtlSamplerState() { return m_mtlSamplerState; }

    ~MetalSampler() = default;

private:
    id<MTLSamplerState> m_mtlSamplerState = nullptr;

public:
    MetalSampler& operator = (const MetalSampler&) = delete;
    MetalSampler& operator = (MetalSampler&&)      = delete;
};

}

#endif // METALSAMPLER_HPP