/*
 * ---------------------------------------------------
 * MeshLoader.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/21 18:53:38
 * ---------------------------------------------------
 */

#include "MeshLoader.hpp"

#include <Graphics/Device.hpp>

#include <cassert>
#include <memory>

namespace scop
{

constexpr std::array<Vertex, 24> cube_vertices = {
    // Front face (+Z)
    Vertex{ .pos=glm::vec3(-0.5f, -0.5f,  0.5f), .uv=glm::vec2(0.0f, 1.0f) },
    Vertex{ .pos=glm::vec3( 0.5f, -0.5f,  0.5f), .uv=glm::vec2(1.0f, 1.0f) },
    Vertex{ .pos=glm::vec3( 0.5f,  0.5f,  0.5f), .uv=glm::vec2(1.0f, 0.0f) },
    Vertex{ .pos=glm::vec3(-0.5f,  0.5f,  0.5f), .uv=glm::vec2(0.0f, 0.0f) },
    // Back face (-Z)
    Vertex{ .pos=glm::vec3( 0.5f, -0.5f, -0.5f), .uv=glm::vec2(0.0f, 1.0f) },
    Vertex{ .pos=glm::vec3(-0.5f, -0.5f, -0.5f), .uv=glm::vec2(1.0f, 1.0f) },
    Vertex{ .pos=glm::vec3(-0.5f,  0.5f, -0.5f), .uv=glm::vec2(1.0f, 0.0f) },
    Vertex{ .pos=glm::vec3( 0.5f,  0.5f, -0.5f), .uv=glm::vec2(0.0f, 0.0f) },
    // Left face (-X)
    Vertex{ .pos=glm::vec3(-0.5f, -0.5f, -0.5f), .uv=glm::vec2(0.0f, 1.0f) },
    Vertex{ .pos=glm::vec3(-0.5f, -0.5f,  0.5f), .uv=glm::vec2(1.0f, 1.0f) },
    Vertex{ .pos=glm::vec3(-0.5f,  0.5f,  0.5f), .uv=glm::vec2(1.0f, 0.0f) },
    Vertex{ .pos=glm::vec3(-0.5f,  0.5f, -0.5f), .uv=glm::vec2(0.0f, 0.0f) },
    // Right face (+X)
    Vertex{ .pos=glm::vec3( 0.5f, -0.5f,  0.5f), .uv=glm::vec2(0.0f, 1.0f) },
    Vertex{ .pos=glm::vec3( 0.5f, -0.5f, -0.5f), .uv=glm::vec2(1.0f, 1.0f) },
    Vertex{ .pos=glm::vec3( 0.5f,  0.5f, -0.5f), .uv=glm::vec2(1.0f, 0.0f) },
    Vertex{ .pos=glm::vec3( 0.5f,  0.5f,  0.5f), .uv=glm::vec2(0.0f, 0.0f) },
    // Top face (+Y)
    Vertex{ .pos=glm::vec3(-0.5f,  0.5f,  0.5f), .uv=glm::vec2(0.0f, 1.0f) },
    Vertex{ .pos=glm::vec3( 0.5f,  0.5f,  0.5f), .uv=glm::vec2(1.0f, 1.0f) },
    Vertex{ .pos=glm::vec3( 0.5f,  0.5f, -0.5f), .uv=glm::vec2(1.0f, 0.0f) },
    Vertex{ .pos=glm::vec3(-0.5f,  0.5f, -0.5f), .uv=glm::vec2(0.0f, 0.0f) },
    // Bottom face (-Y)
    Vertex{ .pos=glm::vec3(-0.5f, -0.5f, -0.5f), .uv=glm::vec2(0.0f, 1.0f) },
    Vertex{ .pos=glm::vec3( 0.5f, -0.5f, -0.5f), .uv=glm::vec2(1.0f, 1.0f) },
    Vertex{ .pos=glm::vec3( 0.5f, -0.5f,  0.5f), .uv=glm::vec2(1.0f, 0.0f) },
    Vertex{ .pos=glm::vec3(-0.5f, -0.5f,  0.5f), .uv=glm::vec2(0.0f, 0.0f) }
};

constexpr std::array<uint32_t, 36> cube_indices = {
    // Front face
    0, 1, 2, 0, 2, 3,
    // Back face
    4, 5, 6, 4, 6, 7,
    // Left face
    8, 9,10, 8,10,11,
    // Right face
   12,13,14,12,14,15,
    // Top face
   16,17,18,16,18,19,
    // Bottom face
   20,21,22,20,22,23
};

MeshLoader::MeshLoader(gfx::Device* device)
    : m_device(device)
{
    assert(m_device);
    m_commandBufferPool = m_device->newCommandBufferPool();
    assert(m_commandBufferPool);
}

Mesh MeshLoader::builtinCube(const std::function<std::shared_ptr<Material>()>& mkMaterial)
{
    std::unique_ptr<gfx::CommandBuffer> commandBuffer = m_commandBufferPool->get();
    commandBuffer->beginBlitPass();
    auto mesh = Mesh{
        .name = "cube_mesh",
        .subMeshes = {
            scop::SubMesh{
                .name = "cube_submesh",
                .transform = glm::mat4x4(1),
                .vertexBuffer = newVertexBuffer(cube_vertices, *commandBuffer),
                .indexBuffer = newIndexBuffer(cube_indices, *commandBuffer),
                .material = mkMaterial()
            }
        }
    };
    commandBuffer->endBlitPass();
    m_device->submitCommandBuffers(std::move(commandBuffer));
    return mesh;
}

}
