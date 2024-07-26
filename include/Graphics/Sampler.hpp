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

#include "UtilsCPP/String.hpp"

namespace gfx
{

class Sampler
{
public:
    enum class AddressMode  { ClampToEdge, Repeat, MirrorRepeat };
    enum class MinMagFilter { Nearest, Linear };

    struct Descriptor
    {
        utils::String debugName;
        AddressMode sAddressMode = AddressMode::ClampToEdge;
        AddressMode tAddressMode = AddressMode::ClampToEdge;
        AddressMode rAddressMode = AddressMode::ClampToEdge;
        MinMagFilter minFilter = MinMagFilter::Nearest;
        MinMagFilter magFilter = MinMagFilter::Nearest;
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