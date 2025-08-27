/*
 * ---------------------------------------------------
 * MetalParameterBlockPool.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/10 19:15:57
 * ---------------------------------------------------
 */

#ifndef METALPARAMETERBLOCKPOOL_HPP
#define METALPARAMETERBLOCKPOOL_HPP

#include "Graphics/ParameterBlock.hpp"

#include "Metal/MetalParameterBlock.hpp"
#include "Metal/MetalBuffer.hpp"

namespace gfx
{

class MetalDevice;

class MetalParameterBlockPool
{
public:
    MetalParameterBlockPool() = default;
    MetalParameterBlockPool(const MetalParameterBlockPool&) = delete;
    MetalParameterBlockPool(MetalParameterBlockPool&&) = default;

    MetalParameterBlockPool(const MetalDevice*);

    MetalParameterBlock& get(const ParameterBlock::Layout&);
    void reset();
    void clear();

    ~MetalParameterBlockPool() = default;

private:
    MetalBuffer m_argumentBuffer;
    size_t m_nextOffset = 0;
    ext::deque<MetalParameterBlock> m_parameterBlocks;

public:
    MetalParameterBlockPool& operator=(const MetalParameterBlockPool&) = delete;
    MetalParameterBlockPool& operator=(MetalParameterBlockPool&&) = default;
};

} // namespace gfx

#endif // METALPARAMETERBLOCKPOOL_HPP
