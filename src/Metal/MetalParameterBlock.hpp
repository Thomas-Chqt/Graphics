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

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <map>
    #include <memory>
    namespace ext = std;
#endif

namespace gfx
{

class MetalParameterBlock : public ParameterBlock
{
public:
    MetalParameterBlock() = delete;
    MetalParameterBlock(const MetalParameterBlock&) = delete;
    MetalParameterBlock(MetalParameterBlock&&)      = delete;

    MetalParameterBlock(MetalBuffer*, size_t offset, const ParameterBlock::Layout&);

    void setBinding(uint32_t idx, const ext::shared_ptr<const Buffer>&) override;

    inline const MetalBuffer& argumentBuffer() const { return *m_argumentBuffer; }
    inline size_t offset() const { return m_offset; }
    inline const ext::map<ParameterBlock::Binding, ext::shared_ptr<const MetalBuffer>>& encodedBuffers() const { return m_encodedBuffers; };

    ~MetalParameterBlock() = default;

private:
    MetalBuffer* m_argumentBuffer = nullptr;
    size_t m_offset = 0;
    ParameterBlock::Layout m_layout;

    ext::map<ParameterBlock::Binding, ext::shared_ptr<const MetalBuffer>> m_encodedBuffers;

public:
    MetalParameterBlock& operator = (const MetalParameterBlock&) = delete;
    MetalParameterBlock& operator = (MetalParameterBlock&&)      = delete;
};

}

#endif // METALPARAMETERBLOCK_HPP
