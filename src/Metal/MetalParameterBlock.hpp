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

#include <ranges>
#include <unordered_map>
#include <vector>

namespace gfx
{

class MetalParameterBlock : public ParameterBlock
{
public:
    template<typename T>
    struct EncodedResource
    {
        std::shared_ptr<T> resource;
        ParameterBlockBinding binding;
    };

public:
    MetalParameterBlock() = delete;
    MetalParameterBlock(const MetalParameterBlock&) = delete;
    MetalParameterBlock(MetalParameterBlock&&) = delete;

    MetalParameterBlock(const std::shared_ptr<MetalParameterBlockLayout>&, const std::shared_ptr<MetalBuffer>&, size_t offset);

    inline std::shared_ptr<ParameterBlockLayout> layout() const override { return m_layout; }

    void setBinding(uint32_t idx, const std::shared_ptr<Buffer>&) override;

    void setBinding(uint32_t idx, const std::shared_ptr<Texture>&) override;
    void setBinding(uint32_t idx, uint32_t arrayIndex, const std::shared_ptr<Texture>&) override;
    void setBinding(uint32_t idx, uint32_t firstArrayIndex, std::span<const std::shared_ptr<Texture>>) override;

    void setBinding(uint32_t idx, const std::shared_ptr<Sampler>&) override;

    void clearBinding(uint32_t idx, uint32_t arrayIndex) override;
    void clearBinding(uint32_t idx, uint32_t firstArrayIndex, uint32_t count) override;

    inline const MetalBuffer& argumentBuffer() const { return *m_argumentBuffer; }
    inline size_t offset() const { return m_offset; }

    inline auto encodedBuffers()  const { return m_encodedBuffers  | std::views::transform([](const auto& resources) { return resources | std::views::values; }) | std::views::join; }
    inline auto encodedTextures() const { return m_encodedTextures | std::views::transform([](const auto& resources) { return resources | std::views::values; }) | std::views::join; }
    inline auto encodedSamplers() const { return m_encodedSamplers | std::views::transform([](const auto& resources) { return resources | std::views::values; }) | std::views::join; }

    inline void reuse()
    {
        for (auto& resources : m_encodedBuffers)
            resources.clear();
        for (auto& resources : m_encodedTextures)
            resources.clear();
        for (auto& resources : m_encodedSamplers)
            resources.clear();
    }

    ~MetalParameterBlock() override = default;

private:
    std::shared_ptr<MetalParameterBlockLayout> m_layout;
    std::shared_ptr<MetalBuffer> m_argumentBuffer;
    size_t m_offset = 0;

    std::vector<std::unordered_map<uint32_t, EncodedResource<MetalBuffer>>> m_encodedBuffers;
    std::vector<std::unordered_map<uint32_t, EncodedResource<MetalTexture>>> m_encodedTextures;
    std::vector<std::unordered_map<uint32_t, EncodedResource<MetalSampler>>> m_encodedSamplers;

public:
    MetalParameterBlock& operator=(const MetalParameterBlock&) = delete;
    MetalParameterBlock& operator=(MetalParameterBlock&&) = delete;
};

} // namespace gfx

#endif // METALPARAMETERBLOCK_HPP
