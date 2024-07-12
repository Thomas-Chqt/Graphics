/*
 * ---------------------------------------------------
 * Sampler.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/12 11:53:12
 * ---------------------------------------------------
 */

#ifndef SAMPLER_HPP
# define SAMPLER_HPP

#include "Enums.hpp"
#include "UtilsCPP/String.hpp"

namespace gfx
{

class Sampler
{
public:
    struct Descriptor
    {
        utils::String debugName;
        SamplerAddressMode sAddressMode = SamplerAddressMode::ClampToEdge;
        SamplerAddressMode tAddressMode = SamplerAddressMode::ClampToEdge;
        SamplerMinMagFilter minFilter = SamplerMinMagFilter::Nearest;
        SamplerMinMagFilter magFilter = SamplerMinMagFilter::Nearest;
    };

public:
    Sampler(const Sampler&) = delete;
    Sampler(Sampler&&)      = delete;
    
    virtual ~Sampler() = default;

protected:
    Sampler() = default;
    
public:
    Sampler& operator = (const Sampler&) = delete;
    Sampler& operator = (Sampler&&)      = delete;
};

}

#endif // SAMPLER_HPP