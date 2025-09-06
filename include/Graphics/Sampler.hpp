/*
 * ---------------------------------------------------
 * Sampler.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/15 07:22:06
 * ---------------------------------------------------
 */

#ifndef SAMPLER_HPP
#define SAMPLER_HPP

#include "Graphics/Enums.hpp"

namespace gfx
{

class Sampler
{
public:
    struct Descriptor
    {
        SamplerAddressMode sAddressMode = SamplerAddressMode::ClampToEdge;
        SamplerAddressMode tAddressMode = SamplerAddressMode::ClampToEdge;
        SamplerAddressMode rAddressMode = SamplerAddressMode::ClampToEdge;
        SamplerMinMagFilter minFilter = SamplerMinMagFilter::Nearest;
        SamplerMinMagFilter magFilter = SamplerMinMagFilter::Nearest;
    };

public:
    Sampler(const Sampler&) = delete;
    Sampler(Sampler&&) = delete;

    virtual ~Sampler() = default;

protected:
    Sampler() = default;

public:
    Sampler& operator=(const Sampler&) = delete;
    Sampler& operator=(Sampler&&) = delete;
};

} // namespace gfx

#endif // SAMPLER_HPP
