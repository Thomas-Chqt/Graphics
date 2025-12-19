/*
 * ---------------------------------------------------
 * AssetLoader.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/21 18:49:17
 * ---------------------------------------------------
 */

#ifndef ASSETLOADER_HPP
#define ASSETLOADER_HPP

#include "Mesh.hpp"
#include "Material.hpp"

#include <Graphics/Device.hpp>
#include <Graphics/CommandBuffer.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Buffer.hpp>

#if !defined (SCOP_MANDATORY)
#include <glm/glm.hpp>
#else
#include "math/math.hpp"
#ifndef SCOP_MATH_GLM_ALIAS_DEFINED
#define SCOP_MATH_GLM_ALIAS_DEFINED
namespace glm = scop::math;
#endif
#endif

#include <cstdint>
#include <cassert>
#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <ranges>
#include <vector>

struct aiTexture;

namespace scop
{

class AssetLoader
{
public:
    AssetLoader() = delete;
    AssetLoader(const AssetLoader&) = delete;
    AssetLoader(AssetLoader&&) = delete;

    AssetLoader(gfx::Device*);

    Mesh builtinCube(const std::shared_ptr<Material>&);
    Mesh loadMesh(const std::filesystem::path&, std::optional<std::shared_ptr<Material>> overrideMaterial = std::nullopt);

    std::shared_ptr<gfx::Texture> loadTexture(const std::filesystem::path&, gfx::CommandBuffer&);
    std::shared_ptr<gfx::Texture> loadCubeTexture(const std::filesystem::path& right, const std::filesystem::path& left, const std::filesystem::path& top, const std::filesystem::path& bottom, const std::filesystem::path& front, const std::filesystem::path& back, gfx::CommandBuffer&);
    std::shared_ptr<gfx::Texture> getSolidColorTexture(const glm::vec4&, gfx::CommandBuffer&);

    ~AssetLoader() = default;

private:
    std::shared_ptr<gfx::Texture> loadEmbeddedTexture(const aiTexture*, gfx::CommandBuffer&);

    std::shared_ptr<gfx::Buffer> newVertexBuffer(const std::ranges::range auto& vertices, gfx::CommandBuffer& commandBuffer)
        requires std::same_as<std::ranges::range_value_t<decltype(vertices)>, Vertex>
    {
        std::shared_ptr<gfx::Buffer> vertexBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = sizeof(Vertex) * vertices.size(),
            .usages = gfx::BufferUsage::vertexBuffer | gfx::BufferUsage::copyDestination,
            .storageMode = gfx::ResourceStorageMode::deviceLocal});
        assert(vertexBuffer);

        std::shared_ptr<gfx::Buffer> stagingBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = vertexBuffer->size(),
            .usages = gfx::BufferUsage::copySource,
            .storageMode = gfx::ResourceStorageMode::hostVisible});
        assert(stagingBuffer);

        std::ranges::copy(vertices, stagingBuffer->content<Vertex>());

        commandBuffer.copyBufferToBuffer(stagingBuffer, vertexBuffer, vertexBuffer->size());

        return vertexBuffer;
    }

    std::shared_ptr<gfx::Buffer> newIndexBuffer(const std::ranges::range auto& indices, gfx::CommandBuffer& commandBuffer)
        requires std::same_as<std::ranges::range_value_t<decltype(indices)>, uint32_t>
    {
        std::shared_ptr<gfx::Buffer> indexBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = sizeof(uint32_t) * indices.size(),
            .usages = gfx::BufferUsage::indexBuffer | gfx::BufferUsage::copyDestination,
            .storageMode = gfx::ResourceStorageMode::deviceLocal});
        assert(indexBuffer);

        std::shared_ptr<gfx::Buffer> stagingBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = indexBuffer->size(),
            .usages = gfx::BufferUsage::copySource,
            .storageMode = gfx::ResourceStorageMode::hostVisible});
        assert(stagingBuffer);

        std::ranges::copy(indices, stagingBuffer->content<uint32_t>());

        commandBuffer.copyBufferToBuffer(stagingBuffer, indexBuffer, indexBuffer->size());

        return indexBuffer;
    }

    gfx::Device* m_device;
    std::vector<std::pair<glm::vec4, std::shared_ptr<gfx::Texture>>> m_solidColorTextureCache;
    std::mutex m_solidColorTextureCacheMtx;

public:
    AssetLoader& operator=(const AssetLoader&) = delete;
    AssetLoader& operator=(AssetLoader&&) = delete;
};

} // namespace scop

#endif // ASSETLOADER_HPP
