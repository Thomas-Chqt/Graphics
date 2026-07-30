#pragma once

#include "Graphics/Texture.hpp"

#include "Vulkan/VulkanTexture.hpp"
#include "Vulkan/VulkanSampler.hpp"

namespace gfx
{

class VulkanTextureView : public Texture
{
public:
    using ImTextureIdCleanup = void(*)(uint64_t);

public:
    VulkanTextureView(const VulkanTextureView&) = delete;
    VulkanTextureView(VulkanTextureView&&) = delete;

    VulkanTextureView(const VulkanDevice*, const std::shared_ptr<VulkanTexture>&, const Texture::ViewDescriptor&);
    VulkanTextureView(const VulkanDevice*, const std::shared_ptr<VulkanTextureView>&, const Texture::ViewDescriptor&);

    TextureType type() const override;

    uint32_t width() const override;
    uint32_t height() const override;

    uint32_t baseMipLevel() const override;
    uint32_t mipLevelCount() const override;

    uint32_t baseArrayLayer() const override;
    uint32_t arrayLayerCount() const override;

    PixelFormat pixelFormat() const override;

    TextureUsages usages() const override;
    ResourceStorageMode storageMode() const override;

    std::shared_ptr<VulkanTexture> rootTexture();

    const vk::ImageView& vkImageView() const;
    const vk::ImageSubresourceRange& subresourceRange() const;

    void setImTextureId(uint64_t, const std::shared_ptr<VulkanSampler>&, ImTextureIdCleanup);
    std::optional<uint64_t> imTextureId() const;
    void removeImTextureId();

    ~VulkanTextureView() override;

protected:
    const VulkanDevice* m_device = nullptr;

private:
    std::shared_ptr<VulkanTexture> m_rootTexture;

    TextureType m_type;

    uint32_t m_baseMipLevel;
    uint32_t m_mipLevelCount;

    uint32_t m_baseArrayLayer;
    uint32_t m_arrayLayerCount;

    vk::ImageSubresourceRange m_subresourceRange;
    vk::ImageView m_vkImageView;

    std::optional<uint64_t> m_imTextureId;
    ImTextureIdCleanup m_imTextureIdCleanup = nullptr;
    std::shared_ptr<VulkanSampler> m_imTextureIdSampler;

public:
    VulkanTextureView& operator=(const VulkanTextureView&) = delete;
    VulkanTextureView& operator=(VulkanTextureView&&) = delete;
};

} // namespace gfx
