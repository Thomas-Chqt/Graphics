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
#include "Metal/MetalTexture.hpp"
#include "Metal/MetalSampler.hpp"

namespace gfx
{

class MetalParameterBlockPool;

class MetalParameterBlock : public ParameterBlock
{
public:
    MetalParameterBlock() = delete;
    MetalParameterBlock(const MetalParameterBlock&) = delete;
    MetalParameterBlock(MetalParameterBlock&&) = delete;

    MetalParameterBlock(const ext::shared_ptr<MetalBuffer>&, size_t offset, const ParameterBlock::Layout&, MetalParameterBlockPool*);

    void setBinding(uint32_t idx, const ext::shared_ptr<Buffer>&) override;
    void setBinding(uint32_t idx, const ext::shared_ptr<Texture>&) override;
    void setBinding(uint32_t idx, const ext::shared_ptr<Sampler>&) override;

    inline const MetalBuffer& argumentBuffer() const { return *m_argumentBuffer; }
    inline size_t offset() const { return m_offset; }
    inline const ParameterBlock::Layout& layout() const { return m_layout; }
    inline void clearSourcePool() { m_sourcePool = nullptr; }

    inline const ext::map<ext::shared_ptr<MetalBuffer>, ParameterBlock::Binding>& encodedBuffers() const { return m_encodedBuffers; }
    inline const ext::map<ext::shared_ptr<MetalTexture>, ParameterBlock::Binding>& encodedTextures() const { return m_encodedTextures; };
    inline const ext::map<ext::shared_ptr<MetalSampler>, ParameterBlock::Binding>& encodedSamplers() const { return m_encodedSamplers; };

    ~MetalParameterBlock() override;

private:
    ext::shared_ptr<MetalBuffer> m_argumentBuffer;
    size_t m_offset = 0;
    ParameterBlock::Layout m_layout;
    MetalParameterBlockPool* m_sourcePool;

    ext::map<ext::shared_ptr<MetalBuffer>, ParameterBlock::Binding> m_encodedBuffers;
    ext::map<ext::shared_ptr<MetalTexture>, ParameterBlock::Binding> m_encodedTextures;
    ext::map<ext::shared_ptr<MetalSampler>, ParameterBlock::Binding> m_encodedSamplers;

public:
    MetalParameterBlock& operator=(const MetalParameterBlock&) = delete;
    MetalParameterBlock& operator=(MetalParameterBlock&&) = delete;
};

} // namespace gfx

#endif // METALPARAMETERBLOCK_HPP
