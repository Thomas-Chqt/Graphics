/*
 * ---------------------------------------------------
 * VulkanParameterBlock.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/10 10:51:12
 * ---------------------------------------------------
 */

#ifndef VULKANPARAMETERBLOCK_HPP
#define VULKANPARAMETERBLOCK_HPP

#include "Graphics/Buffer.hpp"
#include "Graphics/ParameterBlock.hpp"

#include "Vulkan/VulkanBuffer.hpp"
#include "Vulkan/VulkanTexture.hpp"
#include "Vulkan/VulkanSampler.hpp"
#include "Vulkan/VulkanParameterBlockLayout.hpp"

#include <ranges>
#include <unordered_map>
#include <vector>

namespace gfx
{

class VulkanDevice;

class VulkanParameterBlock : public ParameterBlock
{
public:
    template<typename T>
    struct UsedResource
    {
        std::shared_ptr<T> resource;
        ParameterBlockBinding binding;
    };

public:
    VulkanParameterBlock() = default;
    VulkanParameterBlock(const VulkanParameterBlock&) = delete;
    VulkanParameterBlock(VulkanParameterBlock&&) = delete;

    VulkanParameterBlock(const VulkanDevice*, const std::shared_ptr<VulkanParameterBlockLayout>&, const std::shared_ptr<vk::DescriptorPool>&);

    inline std::shared_ptr<ParameterBlockLayout> layout() const override { return m_layout; }

    void setBinding(uint32_t idx, const std::shared_ptr<Buffer>&) override;

    void setBinding(uint32_t idx, const std::shared_ptr<Texture>&) override;
    void setBinding(uint32_t idx, uint32_t arrayIndex, const std::shared_ptr<Texture>&) override;
    void setBinding(uint32_t idx, uint32_t firstArrayIndex, std::span<const std::shared_ptr<Texture>>) override;

    void setBinding(uint32_t idx, const std::shared_ptr<Sampler>&) override;

    void clearBinding(uint32_t idx, uint32_t arrayIndex) override;
    void clearBinding(uint32_t idx, uint32_t firstArrayIndex, uint32_t count) override;

    inline const vk::DescriptorSet& descriptorSet() const { return m_descriptorSet; }

    inline auto usedBuffers()  const { return m_usedBuffers  | std::views::transform([](const auto& resources) { return resources | std::views::values; }) | std::views::join; }
    inline auto usedTextures() const { return m_usedTextures | std::views::transform([](const auto& resources) { return resources | std::views::values; }) | std::views::join; }
    inline auto usedSamplers() const { return m_usedSamplers | std::views::transform([](const auto& resources) { return resources | std::views::values; }) | std::views::join; }

    ~VulkanParameterBlock() override = default;

private:
    const VulkanDevice* m_device = nullptr;
    std::shared_ptr<VulkanParameterBlockLayout> m_layout;
    std::shared_ptr<vk::DescriptorPool> m_descriptorPool;

    vk::DescriptorSet m_descriptorSet;

    std::vector<std::unordered_map<uint32_t, UsedResource<VulkanBuffer>>> m_usedBuffers;
    std::vector<std::unordered_map<uint32_t, UsedResource<VulkanTexture>>> m_usedTextures;
    std::vector<std::unordered_map<uint32_t, UsedResource<VulkanSampler>>> m_usedSamplers;

public:
    VulkanParameterBlock& operator=(const VulkanParameterBlock&) = delete;
    VulkanParameterBlock& operator=(VulkanParameterBlock&&) = default;
};

} // namespace gfx

#endif // VULKANPARAMETERBLOCK_HPP
