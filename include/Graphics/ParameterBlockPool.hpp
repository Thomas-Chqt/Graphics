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

#if defined(GFX_USE_UTILSCPP)
#else
    #include <memory>
#endif

namespace gfx
{

class ParameterBlockPool
{
public:
    ParameterBlockPool(const ParameterBlockPool&) = delete;
    ParameterBlockPool(ParameterBlockPool&&) = delete;

    virtual ext::unique_ptr<ParameterBlock> get(const ParameterBlock::Layout&) = 0;

    virtual ~ParameterBlockPool() = default;

protected:
    ParameterBlockPool() = default;

public:
    ParameterBlockPool& operator=(const ParameterBlockPool&) = delete;
    ParameterBlockPool& operator=(ParameterBlockPool&&) = delete;
};

} // namespace gfx

#endif // PARAMETERBLOCKPOOL_HPP
