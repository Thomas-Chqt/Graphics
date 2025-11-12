/*
 * ---------------------------------------------------
 * ParameterBlock.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/10 10:02:57
 * ---------------------------------------------------
 */

#ifndef PARAMETERBLOCK_HPP
#define PARAMETERBLOCK_HPP

#include "Graphics/Enums.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Sampler.hpp"
#include "ParameterBlockLayout.hpp"

#include <cstdint>
#include <memory>

namespace gfx
{

class ParameterBlock
{
public:
    ParameterBlock(const ParameterBlock&) = delete;

    virtual std::shared_ptr<ParameterBlockLayout> layout() const = 0;

    virtual void setBinding(uint32_t idx, const std::shared_ptr<Buffer>&) = 0;
    virtual void setBinding(uint32_t idx, const std::shared_ptr<Texture>&) = 0;
    virtual void setBinding(uint32_t idx, const std::shared_ptr<Sampler>&) = 0;

    virtual ~ParameterBlock() = default;

protected:
    ParameterBlock(ParameterBlock&&) = default;
    ParameterBlock() = default;

public:
    ParameterBlock& operator=(const ParameterBlock&) = delete;

protected:
    ParameterBlock& operator=(ParameterBlock&&) = default;
};

}

#endif // PARAMETERBLOCK_HPP
