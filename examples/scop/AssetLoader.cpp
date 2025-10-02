/*
 * ---------------------------------------------------
 * AssetLoader.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/21 18:53:38
 * ---------------------------------------------------
 */

#include "AssetLoader.hpp"
#include "Graphics/Texture.hpp"

#include <Graphics/Device.hpp>

#include <cstring>
#include <filesystem>
#include <map>
#include <stb_image/stb_image.h>

#include <cassert>
#include <memory>
#include <stdexcept>

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
    UniqueStbiUc bytes = UniqueStbiUc(stbi_load(path.c_str(), &width, &height, nullptr, STBI_rgb_alpha), stbi_image_free);

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

    UniqueStbiUc img(stbi_load(right.c_str(), &width, &height, nullptr, STBI_rgb_alpha), stbi_image_free);
    if (!img)
        throw std::runtime_error("failed to load cube face: " + right.string());
    bytes.emplace(right, std::move(img));
    for (const auto& path : { left, top, bottom, front, back })
    {
        if (bytes.contains(path))
            continue;
        int w = 0, h = 0;
        UniqueStbiUc img(stbi_load(path.c_str(), &w, &h, nullptr, STBI_rgb_alpha), stbi_image_free);
        if (!img)
            throw std::runtime_error("failed to load cube face: " + path.string());
        if (w != width || h != height)
            throw std::runtime_error("all images of a cube must have the same size");
        bytes.emplace(path, std::move(img));
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
