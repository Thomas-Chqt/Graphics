/*
 * ---------------------------------------------------
 * AssetLoader.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/21 18:53:38
 * ---------------------------------------------------
 */

#include "AssetLoader.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/Texture.hpp"
#include "Material.hpp"

#include <Graphics/Device.hpp>

#include <glm/ext/matrix_transform.hpp>
#include <stb_image/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/types.h>

#include <array>
#include <cstdint>
#include <map>
#include <ranges>
#include <cstring>
#include <filesystem>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <vector>

namespace scop
{

constexpr std::array<Vertex, 24> cube_vertices = {
    // Front face (+Z)
    Vertex{ .pos=glm::vec3(-0.5f, -0.5f,  0.5f), .uv=glm::vec2(0.0f, 1.0f), .normal={ 0,  0,  1} },
    Vertex{ .pos=glm::vec3( 0.5f, -0.5f,  0.5f), .uv=glm::vec2(1.0f, 1.0f), .normal={ 0,  0,  1} },
    Vertex{ .pos=glm::vec3( 0.5f,  0.5f,  0.5f), .uv=glm::vec2(1.0f, 0.0f), .normal={ 0,  0,  1} },
    Vertex{ .pos=glm::vec3(-0.5f,  0.5f,  0.5f), .uv=glm::vec2(0.0f, 0.0f), .normal={ 0,  0,  1} },
    // Back face (-Z)
    Vertex{ .pos=glm::vec3( 0.5f, -0.5f, -0.5f), .uv=glm::vec2(0.0f, 1.0f), .normal={ 0,  0, -1} },
    Vertex{ .pos=glm::vec3(-0.5f, -0.5f, -0.5f), .uv=glm::vec2(1.0f, 1.0f), .normal={ 0,  0, -1} },
    Vertex{ .pos=glm::vec3(-0.5f,  0.5f, -0.5f), .uv=glm::vec2(1.0f, 0.0f), .normal={ 0,  0, -1} },
    Vertex{ .pos=glm::vec3( 0.5f,  0.5f, -0.5f), .uv=glm::vec2(0.0f, 0.0f), .normal={ 0,  0, -1} },
    // Left face (-X)
    Vertex{ .pos=glm::vec3(-0.5f, -0.5f, -0.5f), .uv=glm::vec2(0.0f, 1.0f), .normal={-1,  0,  0} },
    Vertex{ .pos=glm::vec3(-0.5f, -0.5f,  0.5f), .uv=glm::vec2(1.0f, 1.0f), .normal={-1,  0,  0} },
    Vertex{ .pos=glm::vec3(-0.5f,  0.5f,  0.5f), .uv=glm::vec2(1.0f, 0.0f), .normal={-1,  0,  0} },
    Vertex{ .pos=glm::vec3(-0.5f,  0.5f, -0.5f), .uv=glm::vec2(0.0f, 0.0f), .normal={-1,  0,  0} },
    // Right face (+X)
    Vertex{ .pos=glm::vec3( 0.5f, -0.5f,  0.5f), .uv=glm::vec2(0.0f, 1.0f), .normal={ 1,  0,  0} },
    Vertex{ .pos=glm::vec3( 0.5f, -0.5f, -0.5f), .uv=glm::vec2(1.0f, 1.0f), .normal={ 1,  0,  0} },
    Vertex{ .pos=glm::vec3( 0.5f,  0.5f, -0.5f), .uv=glm::vec2(1.0f, 0.0f), .normal={ 1,  0,  0} },
    Vertex{ .pos=glm::vec3( 0.5f,  0.5f,  0.5f), .uv=glm::vec2(0.0f, 0.0f), .normal={ 1,  0,  0} },
    // Top face (+Y)
    Vertex{ .pos=glm::vec3(-0.5f,  0.5f,  0.5f), .uv=glm::vec2(0.0f, 1.0f), .normal={ 0,  1,  0} },
    Vertex{ .pos=glm::vec3( 0.5f,  0.5f,  0.5f), .uv=glm::vec2(1.0f, 1.0f), .normal={ 0,  1,  0} },
    Vertex{ .pos=glm::vec3( 0.5f,  0.5f, -0.5f), .uv=glm::vec2(1.0f, 0.0f), .normal={ 0,  1,  0} },
    Vertex{ .pos=glm::vec3(-0.5f,  0.5f, -0.5f), .uv=glm::vec2(0.0f, 0.0f), .normal={ 0,  1,  0} },
    // Bottom face (-Y)
    Vertex{ .pos=glm::vec3(-0.5f, -0.5f, -0.5f), .uv=glm::vec2(0.0f, 1.0f), .normal={ 0, -1,  0} },
    Vertex{ .pos=glm::vec3( 0.5f, -0.5f, -0.5f), .uv=glm::vec2(1.0f, 1.0f), .normal={ 0, -1,  0} },
    Vertex{ .pos=glm::vec3( 0.5f, -0.5f,  0.5f), .uv=glm::vec2(1.0f, 0.0f), .normal={ 0, -1,  0} },
    Vertex{ .pos=glm::vec3(-0.5f, -0.5f,  0.5f), .uv=glm::vec2(0.0f, 0.0f), .normal={ 0, -1,  0} }
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

constexpr unsigned int POST_PROCESSING_FLAGS = aiProcess_JoinIdenticalVertices |
                                               aiProcess_MakeLeftHanded |
                                               aiProcess_Triangulate |
                                               aiProcess_GenSmoothNormals |
                                               aiProcess_FixInfacingNormals |
                                               aiProcess_FlipUVs |
                                               aiProcess_FlipWindingOrder |
                                               aiProcess_CalcTangentSpace;

namespace
{
    static inline glm::mat4x4 toGlmMat4(const aiMatrix4x4& from)
    {
        glm::mat4x4 to;
        to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
        to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
        to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
        to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
        return to;
    }
}

AssetLoader::AssetLoader(gfx::Device* device)
    : m_device(device)
{
    assert(m_device);
    m_commandBufferPool = m_device->newCommandBufferPool();
    assert(m_commandBufferPool);
}

Mesh AssetLoader::builtinCube(const std::function<std::shared_ptr<Material>()>& mkMaterial)
{
    std::unique_ptr<gfx::CommandBuffer> commandBuffer = m_commandBufferPool->get();
    commandBuffer->beginBlitPass();
    auto mesh = Mesh{
        .name = "cube_mesh",
        .subMeshes = {
            SubMesh{
                .name = "cube_submesh",
                .transform = glm::mat4x4(1),
                .vertexBuffer = newVertexBuffer(cube_vertices, *commandBuffer),
                .indexBuffer = newIndexBuffer(cube_indices, *commandBuffer),
                .material = mkMaterial(),
            }
        }
    };
    commandBuffer->endBlitPass();
    m_device->submitCommandBuffers(std::move(commandBuffer));
    return mesh;
}


Mesh AssetLoader::loadMesh(const std::filesystem::path& path, const std::function<std::shared_ptr<Material>()>& mkMaterial)
{
    assert(std::filesystem::is_regular_file(path));

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path.string(), POST_PROCESSING_FLAGS);
    if (scene == nullptr)
        throw std::runtime_error("fail to load the model using assimp");

    std::shared_ptr<Material> material = mkMaterial();

    std::unique_ptr<gfx::CommandBuffer> commandBuffer = m_commandBufferPool->get();
    commandBuffer->beginBlitPass();

    auto flatSubMeshes = std::span(scene->mMeshes, scene->mNumMeshes) | std::views::transform([&](aiMesh* aiMesh) -> SubMesh{
        return SubMesh{
            .name = aiMesh->mName.C_Str(),
            .transform = glm::mat4x4(1.0f),
            .vertexBuffer = newVertexBuffer(std::views::iota(0u, aiMesh->mNumVertices) | std::views::transform([aiMesh](uint32_t i) -> Vertex{
                return Vertex{
                    .pos = glm::vec3(aiMesh->mVertices[i].x, aiMesh->mVertices[i].y, aiMesh->mVertices[i].z),
                    .uv = aiMesh->mTextureCoords[0] != nullptr ? glm::vec2(aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y) : glm::vec2(0.0f),
                    .normal = aiMesh->mNormals != nullptr ? glm::vec3(aiMesh->mNormals[i].x, aiMesh->mNormals[i].y, aiMesh->mNormals[i].z) : glm::vec3(0.0f),
                    .tangent = aiMesh->mTangents != nullptr ? glm::vec3(aiMesh->mTangents[i].x, aiMesh->mTangents[i].y, aiMesh->mTangents[i].z) : glm::vec3(0.0f)
                };
            }), *commandBuffer),
            .indexBuffer = newIndexBuffer(std::views::iota(0u, aiMesh->mNumFaces * 3) | std::views::transform([aiMesh](uint32_t i) -> uint32_t{
                return aiMesh->mFaces[i / 3].mIndices[i % 3];
            }), *commandBuffer),
            .material = material,
            .subMeshes = {}
        };
    });

    std::function<void(std::vector<SubMesh>&, aiNode*, glm::mat4x4)> addNode = [&](std::vector<SubMesh>& dest, aiNode* aiNode, glm::mat4x4 additionalTransform) {
        glm::mat4x4 transform = additionalTransform * toGlmMat4(aiNode->mTransformation);

        auto subMeshes = std::span(aiNode->mMeshes, aiNode->mNumMeshes) | std::views::transform([&](uint32_t i) -> SubMesh {
            SubMesh submesh = flatSubMeshes[i];
            submesh.transform = transform;
            return submesh;
        });

        for (auto* node : std::span(aiNode->mChildren, aiNode->mNumChildren)) {
             if (subMeshes.empty())
                addNode(dest, node, transform);
             else {
                std::vector<SubMesh> subDest;
                addNode(subDest, node, glm::mat4x4(1.0F));
                subMeshes.front().subMeshes.append_range(subDest);
            }
        }

        dest.append_range(subMeshes);
    };

    Mesh mesh = {
        .name = scene->mRootNode->mName.C_Str(),
        .subMeshes = std::span(scene->mRootNode->mMeshes, scene->mRootNode->mNumMeshes) | std::views::transform([&](uint32_t i) -> SubMesh {
            SubMesh submesh = flatSubMeshes[i];
            submesh.transform = glm::mat4x4(1.0F);
            return submesh;
        }) | std::ranges::to<std::vector>()
    };

    for (auto* node : std::span(scene->mRootNode->mChildren, scene->mRootNode->mNumChildren))
        addNode(mesh.subMeshes, node, glm::mat4x4(1.0F));

    commandBuffer->endBlitPass();
    m_device->submitCommandBuffers(std::move(commandBuffer));

    return mesh;
}

using UniqueStbiUc = std::unique_ptr<stbi_uc, decltype(&stbi_image_free)>;

std::shared_ptr<gfx::Texture> AssetLoader::loadTexture(const std::filesystem::path& path, gfx::CommandBuffer* commandBuffer)
{
    std::unique_ptr<gfx::CommandBuffer> newCommandBuffer;
    if (commandBuffer == nullptr) {
        newCommandBuffer = m_commandBufferPool->get();
        commandBuffer = newCommandBuffer.get();
    }
    int width = 0;
    int height = 0;
    UniqueStbiUc bytes = UniqueStbiUc(stbi_load(path.string().c_str(), &width, &height, nullptr, STBI_rgb_alpha), stbi_image_free);

    std::shared_ptr<gfx::Texture> texture = m_device->newTexture(gfx::Texture::Descriptor{
        .type = gfx::TextureType::texture2d,
        .width = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
        .pixelFormat = gfx::PixelFormat::RGBA8Unorm,
        .usages = gfx::TextureUsage::copyDestination | gfx::TextureUsage::shaderRead,
        .storageMode = gfx::ResourceStorageMode::deviceLocal
    });
    assert(texture);

    ext::shared_ptr<gfx::Buffer> stagingBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
        .size = static_cast<size_t>(width) * static_cast<size_t>(height) * pixelFormatSize(gfx::PixelFormat::RGBA8Unorm),
        .usages = gfx::BufferUsage::copySource,
        .storageMode = gfx::ResourceStorageMode::hostVisible
    });
    assert(stagingBuffer);

    std::memcpy(stagingBuffer->content<stbi_uc>(), bytes.get(), stagingBuffer->size());

    if (newCommandBuffer != nullptr)
        newCommandBuffer->beginBlitPass();

    commandBuffer->copyBufferToTexture(stagingBuffer, texture);

    if (newCommandBuffer != nullptr) {
        newCommandBuffer->endBlitPass();
        m_device->submitCommandBuffers(std::move(newCommandBuffer));
    }

    return texture;
}

std::shared_ptr<gfx::Texture> AssetLoader::loadCubeTexture(const std::filesystem::path& right,
                                                           const std::filesystem::path& left,
                                                           const std::filesystem::path& top,
                                                           const std::filesystem::path& bottom,
                                                           const std::filesystem::path& front,
                                                           const std::filesystem::path& back,
                                                           gfx::CommandBuffer* commandBuffer)
{
    std::unique_ptr<gfx::CommandBuffer> newCommandBuffer;
    if (commandBuffer == nullptr) {
        newCommandBuffer = m_commandBufferPool->get();
        commandBuffer = newCommandBuffer.get();
    }
    int width = 0;
    int height = 0;
    std::map<std::filesystem::path, UniqueStbiUc> bytes;

    UniqueStbiUc img(stbi_load(right.string().c_str(), &width, &height, nullptr, STBI_rgb_alpha), stbi_image_free);
    if (!img)
        throw std::runtime_error("failed to load cube face: " + right.string());
    bytes.emplace(right, std::move(img));
    for (const auto& path : { left, top, bottom, front, back })
    {
        if (bytes.contains(path))
            continue;
        int w = 0, h = 0;
        UniqueStbiUc img2(stbi_load(path.string().c_str(), &w, &h, nullptr, STBI_rgb_alpha), stbi_image_free);
        if (!img2)
            throw std::runtime_error("failed to load cube face: " + path.string());
        if (w != width || h != height)
            throw std::runtime_error("all images of a cube must have the same size");
        bytes.emplace(path, std::move(img2));
    }

    std::shared_ptr<gfx::Texture> texture = m_device->newTexture(gfx::Texture::Descriptor{
        .type = gfx::TextureType::textureCube,
        .width = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
        .pixelFormat = gfx::PixelFormat::RGBA8Unorm,
        .usages = gfx::TextureUsage::copyDestination | gfx::TextureUsage::shaderRead,
        .storageMode = gfx::ResourceStorageMode::deviceLocal
    });
    assert(texture);

    size_t faceSize = static_cast<size_t>(width) * static_cast<size_t>(height) * pixelFormatSize(gfx::PixelFormat::RGBA8Unorm);
    ext::shared_ptr<gfx::Buffer> stagingBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
        .size = faceSize * 6, // 6 faces
        .usages = gfx::BufferUsage::copySource,
        .storageMode = gfx::ResourceStorageMode::hostVisible
    });
    assert(stagingBuffer);

    auto* bufferData = stagingBuffer->content<stbi_uc>();
    ext::memcpy(bufferData + 0 * faceSize, bytes.at(right).get(), faceSize);  // +X (right)
    ext::memcpy(bufferData + 1 * faceSize, bytes.at(left).get(), faceSize);   // -X (left)
    ext::memcpy(bufferData + 2 * faceSize, bytes.at(top).get(), faceSize);    // +Y (top)
    ext::memcpy(bufferData + 3 * faceSize, bytes.at(bottom).get(), faceSize); // -Y (bottom)
    ext::memcpy(bufferData + 4 * faceSize, bytes.at(front).get(), faceSize);  // +Z (front)
    ext::memcpy(bufferData + 5 * faceSize, bytes.at(back).get(), faceSize);   // -Z (back)

    if (newCommandBuffer != nullptr)
        newCommandBuffer->beginBlitPass();

    for (int face = 0; face < 6; ++face)
        commandBuffer->copyBufferToTexture(stagingBuffer, face * faceSize, texture, face);

    if (newCommandBuffer != nullptr) {
        newCommandBuffer->endBlitPass();
        m_device->submitCommandBuffers(std::move(newCommandBuffer));
    }

    return texture;
}

}
