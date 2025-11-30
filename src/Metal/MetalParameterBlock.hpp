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
#include "MetalParameterBlockLayout.hpp"

namespace gfx
{

class MetalParameterBlock : public ParameterBlock
{
public:
    MetalParameterBlock() = delete;
    MetalParameterBlock(const MetalParameterBlock&) = delete;
    MetalParameterBlock(MetalParameterBlock&&) = delete;

    MetalParameterBlock(const std::shared_ptr<MetalParameterBlockLayout>&, const std::shared_ptr<MetalBuffer>&, size_t offset);

    inline std::shared_ptr<ParameterBlockLayout> layout() const override { return m_layout; }

    void setBinding(uint32_t idx, const std::shared_ptr<Buffer>&) override;
    void setBinding(uint32_t idx, const std::shared_ptr<Texture>&) override;
    void setBinding(uint32_t idx, const std::shared_ptr<Sampler>&) override;

    inline const MetalBuffer& argumentBuffer() const { return *m_argumentBuffer; }
    inline size_t offset() const { return m_offset; }

    inline const std::map<std::shared_ptr<MetalBuffer>, ParameterBlockBinding>& encodedBuffers() const { return m_nonReusedRessources.encodedBuffers; }
    inline const std::map<std::shared_ptr<MetalTexture>, ParameterBlockBinding>& encodedTextures() const { return m_nonReusedRessources.encodedTextures; };
    inline const std::map<std::shared_ptr<MetalSampler>, ParameterBlockBinding>& encodedSamplers() const { return m_nonReusedRessources.encodedSamplers; };

    inline void reuse() { m_nonReusedRessources = NonReusedRessources(); }

    ~MetalParameterBlock() override = default;

private:
    std::shared_ptr<MetalParameterBlockLayout> m_layout;
    std::shared_ptr<MetalBuffer> m_argumentBuffer;
    size_t m_offset = 0;

    struct NonReusedRessources
    {
        std::map<std::shared_ptr<MetalBuffer>, ParameterBlockBinding> encodedBuffers;
        std::map<std::shared_ptr<MetalTexture>, ParameterBlockBinding> encodedTextures;
        std::map<std::shared_ptr<MetalSampler>, ParameterBlockBinding> encodedSamplers;
    }
    m_nonReusedRessources;

public:
    MetalParameterBlock& operator=(const MetalParameterBlock&) = delete;
    MetalParameterBlock& operator=(MetalParameterBlock&&) = delete;
};

} // namespace gfx

#endif // METALPARAMETERBLOCK_HPP
