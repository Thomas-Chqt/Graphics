/*
 * ---------------------------------------------------
 * MetalSampler.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/12 13:08:10
 * ---------------------------------------------------
 */

#ifndef METALSAMPLER_HPP
#define METALSAMPLER_HPP

#include "Graphics/Sampler.hpp"

namespace gfx
{

class MetalDevice;

class MetalSampler : public Sampler
{
public:
    MetalSampler() = default;
    MetalSampler(const MetalSampler&) = delete;
    MetalSampler(MetalSampler&&) noexcept;

    MetalSampler(const MetalDevice&, const Sampler::Descriptor&);

    inline const id<MTLSamplerState>& mtlSamplerState() const { return m_mtlSamplerState; }

    ~MetalSampler() override;

private:
    id<MTLSamplerState> m_mtlSamplerState = nil;

public:
    MetalSampler& operator=(const MetalSampler&) = delete;
    MetalSampler& operator=(MetalSampler&&) noexcept;
};

} // namespace gfx

#endif // METALSAMPLER_HPP
