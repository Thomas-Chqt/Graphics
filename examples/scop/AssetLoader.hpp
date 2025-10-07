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

#include "Graphics/Texture.hpp"
#include "Material.hpp"

#include <Graphics/Device.hpp>
#include <Graphics/CommandBuffer.hpp>

#include <cstdint>
#include <filesystem>
#include <functional>
#include <memory>
#include <ranges>

#include <glm/glm.hpp>

namespace scop
{

struct Vertex
{
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 normal;
    glm::vec3 tangent;
};

struct SubMesh
{
    std::string name;
    glm::mat4x4 transform;
    std::shared_ptr<gfx::Buffer> vertexBuffer;
    std::shared_ptr<gfx::Buffer> indexBuffer;
    std::shared_ptr<Material> material;
    std::vector<SubMesh> subMeshes;
};

struct Mesh
{
    std::string name;
    std::vector<SubMesh> subMeshes;
};

class AssetLoader
{
public:
    AssetLoader() = delete;
    AssetLoader(const AssetLoader&) = delete;
    AssetLoader(AssetLoader&&) = delete;

    AssetLoader(gfx::Device*);

    Mesh builtinCube(const std::function<std::shared_ptr<Material>()>& mkMaterial);
    Mesh loadMesh(const std::filesystem::path&, const std::function<std::shared_ptr<Material>()>& mkMaterial);

    std::shared_ptr<gfx::Texture> loadTexture(const std::filesystem::path&, gfx::CommandBuffer* = nullptr);
    std::shared_ptr<gfx::Texture> loadCubeTexture(const std::filesystem::path& right,
                                                  const std::filesystem::path& left,
                                                  const std::filesystem::path& top,
                                                  const std::filesystem::path& bottom,
                                                  const std::filesystem::path& front,
                                                  const std::filesystem::path& back,
                                                  gfx::CommandBuffer* = nullptr);

    ~AssetLoader() = default;

private:
    std::shared_ptr<gfx::Buffer> newVertexBuffer(const std::ranges::range auto& vertices, gfx::CommandBuffer& commandBuffer)
        requires std::same_as<std::ranges::range_value_t<decltype(vertices)>, Vertex>
    {
        std::shared_ptr<gfx::Buffer> vertexBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = sizeof(Vertex) * vertices.size(),
            .usages = gfx::BufferUsage::vertexBuffer | gfx::BufferUsage::copyDestination,
            .storageMode = gfx::ResourceStorageMode::deviceLocal});
        assert(vertexBuffer);

        ext::shared_ptr<gfx::Buffer> stagingBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = vertexBuffer->size(),
            .usages = gfx::BufferUsage::copySource,
            .storageMode = gfx::ResourceStorageMode::hostVisible});
        assert(stagingBuffer);

        ext::ranges::copy(vertices, stagingBuffer->content<Vertex>());

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

        ext::shared_ptr<gfx::Buffer> stagingBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = indexBuffer->size(),
            .usages = gfx::BufferUsage::copySource,
            .storageMode = gfx::ResourceStorageMode::hostVisible});
        assert(stagingBuffer);

        ext::ranges::copy(indices, stagingBuffer->content<uint32_t>());

        commandBuffer.copyBufferToBuffer(stagingBuffer, indexBuffer, indexBuffer->size());

        return indexBuffer;
    }

    gfx::Device* m_device;
    std::unique_ptr<gfx::CommandBufferPool> m_commandBufferPool;

public:
    AssetLoader& operator=(const AssetLoader&) = delete;
    AssetLoader& operator=(AssetLoader&&) = delete;
};

} // namespace scop

#endif // ASSETLOADER_HPP
