#pragma once

#include "Metal/MetalTexture.hpp"

#if !defined(__OBJC__)
#error this file can only by used in objective c
#endif

namespace gfx
{

class MetalTextureView final : public MetalTexture
{
public:
    MetalTextureView(const MetalTextureView&) = delete;
    MetalTextureView(MetalTextureView&&) = delete;

    MetalTextureView(const std::shared_ptr<MetalTexture>&, const Texture::ViewDescriptor&);

    TextureUsages usages() const override;
    ResourceStorageMode storageMode() const override;

    std::shared_ptr<MetalTexture> rootTexture();

    ~MetalTextureView() override = default;

private:
    std::shared_ptr<MetalTexture> m_rootTexture;

public:
    MetalTextureView& operator=(const MetalTextureView&) = delete;
    MetalTextureView& operator=(MetalTextureView&&) = delete;
};

} // namespace gfx
