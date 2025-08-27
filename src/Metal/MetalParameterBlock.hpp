/*
 * ---------------------------------------------------
 * MetalParameterBlock.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/10 18:59:24
 * ---------------------------------------------------
 */

#ifndef METALPARAMETERBLOCK_HPP
#define METALPARAMETERBLOCK_HPP

#include "Graphics/Buffer.hpp"
#include "Graphics/ParameterBlock.hpp"

#include "Metal/MetalBuffer.hpp"

namespace gfx
{

class MetalParameterBlock : public ParameterBlock
{
public:
    MetalParameterBlock() = delete;
    MetalParameterBlock(const MetalParameterBlock&) = delete;
    MetalParameterBlock(MetalParameterBlock&&)      = default;

    MetalParameterBlock(MetalBuffer*, size_t offset, const ParameterBlock::Layout&);

    void setBinding(uint32_t idx, const ext::shared_ptr<Buffer>&) override;

    inline const MetalBuffer& argumentBuffer() const { return *m_argumentBuffer; }
    inline size_t offset() const { return m_offset; }
    inline const ext::map<ParameterBlock::Binding, ext::shared_ptr<MetalBuffer>>& encodedBuffers() const { return m_encodedBuffers; };

    ~MetalParameterBlock() override = default;

private:
    MetalBuffer* m_argumentBuffer = nullptr;
    size_t m_offset = 0;
    ext::vector<ParameterBlock::Binding> m_bindings;

    ext::map<ParameterBlock::Binding, ext::shared_ptr<MetalBuffer>> m_encodedBuffers;

public:
    MetalParameterBlock& operator = (const MetalParameterBlock&) = delete;
    MetalParameterBlock& operator = (MetalParameterBlock&&)      = default;
};

}

#endif // METALPARAMETERBLOCK_HPP
