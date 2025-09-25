/*
 * ---------------------------------------------------
 * MeshLoader.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/21 18:49:17
 * ---------------------------------------------------
 */

#ifndef MESHLOADER_HPP
#define MESHLOADER_HPP

#include "Material.hpp"

#include <Graphics/Device.hpp>
#include <Graphics/CommandBuffer.hpp>

#include <cstdint>
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

class MeshLoader
{
public:
    MeshLoader() = delete;
    MeshLoader(const MeshLoader&) = delete;
    MeshLoader(MeshLoader&&) = delete;

    MeshLoader(gfx::Device*);

    Mesh builtinCube(const std::function<std::shared_ptr<Material>()>& mkMaterial);

    ~MeshLoader() = default;

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
    MeshLoader& operator=(const MeshLoader&) = delete;
    MeshLoader& operator=(MeshLoader&&) = delete;
};

} // namespace scop

#endif // MESHLOADER_HPP
