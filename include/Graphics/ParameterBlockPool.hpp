/*
 * ---------------------------------------------------
 * ParameterBlockPool.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/07 10:08:43
 * ---------------------------------------------------
 */

#ifndef PARAMETERBLOCKPOOL_HPP
#define PARAMETERBLOCKPOOL_HPP

#include "Graphics/ParameterBlock.hpp"

#include <cstdint>
#include <memory>

namespace gfx
{

class ParameterBlockPool
{
public:
    struct Descriptor
    {
        uint32_t maxUniformBuffers = 16;
        uint32_t maxTextures = 4;
        uint32_t maxSamplers = 4;
    };

    ParameterBlockPool(const ParameterBlockPool&) = delete;
    ParameterBlockPool(ParameterBlockPool&&) = delete;

    virtual std::unique_ptr<ParameterBlock> get(const ParameterBlock::Layout&) = 0;

    virtual ~ParameterBlockPool() = default;

protected:
    ParameterBlockPool() = default;

public:
    ParameterBlockPool& operator=(const ParameterBlockPool&) = delete;
    ParameterBlockPool& operator=(ParameterBlockPool&&) = delete;
};

} // namespace gfx

#endif // PARAMETERBLOCKPOOL_HPP
