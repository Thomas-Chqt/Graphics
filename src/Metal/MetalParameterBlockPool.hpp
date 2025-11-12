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

#include "Graphics/ParameterBlockPool.hpp"
#include "Graphics/ParameterBlock.hpp"

#include "Metal/MetalParameterBlock.hpp"
#include "Metal/MetalBuffer.hpp"

namespace gfx
{

class MetalDevice;

class MetalParameterBlockPool : public ParameterBlockPool
{
public:
    MetalParameterBlockPool() = delete;
    MetalParameterBlockPool(const MetalParameterBlockPool&) = delete;
    MetalParameterBlockPool(MetalParameterBlockPool&&) = delete;

    MetalParameterBlockPool(const MetalDevice*, const ParameterBlockPool::Descriptor&);

    std::unique_ptr<ParameterBlock> get(const std::shared_ptr<ParameterBlockLayout>&) override;
    void release(ParameterBlock*);

    ~MetalParameterBlockPool() override;

private:
    const MetalDevice* m_device = nullptr; // to allow creating more argument buffers in the future

    std::shared_ptr<MetalBuffer> m_argumentBuffer; // blocks can outlive the pool, only the argument buffer need to remain alive
    size_t m_nextOffset = 0;

    std::set<MetalParameterBlock*> m_usedParameterBlocks;

    std::mutex m_mutex;

public:
    MetalParameterBlockPool& operator=(const MetalParameterBlockPool&) = delete;
    MetalParameterBlockPool& operator=(MetalParameterBlockPool&&) = delete;
};

} // namespace gfx

#endif // METALPARAMETERBLOCKPOOL_HPP
