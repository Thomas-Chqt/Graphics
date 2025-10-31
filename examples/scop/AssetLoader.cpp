/*
 * ---------------------------------------------------
 * AssetLoader.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/21 18:53:38
 * ---------------------------------------------------
 */

#include "AssetLoader.hpp"
#include "Mesh.hpp"
#include "Vertex.hpp"
#include "Material.hpp"

#include <Graphics/Device.hpp>
#include <Graphics/CommandBuffer.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Buffer.hpp>

#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <glm/ext/matrix_transform.hpp>
#include <stb_image/stb_image.h>

#include <algorithm>
#include <bit>
#include <print>
#include <format>
#include <functional>
#include <span>
#include <array>
#include <cstdint>
#include <map>
#include <ranges>
#include <cstring>
#include <filesystem>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

template<>
struct std::formatter<aiMaterial*> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const aiMaterial* mat, std::format_context& ctx) const {
        std::string result;
        result += std::format("name: {}\n", mat->GetName().C_Str());

        int twoSided = 0;
        if (aiGetMaterialInteger(mat, AI_MATKEY_TWOSIDED, &twoSided) == AI_SUCCESS)
            result += std::format("  twosided: {}\n", twoSided);

        int shadingModel = 0;
        if (aiGetMaterialInteger(mat, AI_MATKEY_SHADING_MODEL, &shadingModel) == AI_SUCCESS) {
            const char* shadingName = "Unknown";
            switch (shadingModel) {
                case aiShadingMode_Flat: shadingName = "Flat"; break;
                case aiShadingMode_Gouraud: shadingName = "Gouraud"; break;
                case aiShadingMode_Phong: shadingName = "Phong"; break;
                case aiShadingMode_Blinn: shadingName = "Blinn"; break;
                case aiShadingMode_Toon: shadingName = "Toon"; break;
                case aiShadingMode_OrenNayar: shadingName = "OrenNayar"; break;
                case aiShadingMode_Minnaert: shadingName = "Minnaert"; break;
                case aiShadingMode_CookTorrance: shadingName = "CookTorrance"; break;
                case aiShadingMode_NoShading: shadingName = "NoShading/Unlit"; break;
                case aiShadingMode_Fresnel: shadingName = "Fresnel"; break;
                case aiShadingMode_PBR_BRDF: shadingName = "PBR_BRDF"; break;
                default: shadingName = "unknown"; break;
            }
            result += std::format("  shading_model: {} ({})\n", shadingName, shadingModel);
        }

        int wireframe = 0;
        if (aiGetMaterialInteger(mat, AI_MATKEY_ENABLE_WIREFRAME, &wireframe) == AI_SUCCESS)
            result += std::format("  wireframe: {}\n", wireframe);

        int blendFunc = 0;
        if (aiGetMaterialInteger(mat, AI_MATKEY_BLEND_FUNC, &blendFunc) == AI_SUCCESS)
            result += std::format("  blend_func: {}\n", blendFunc);

        float opacity = 0.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_OPACITY, &opacity) == AI_SUCCESS)
            result += std::format("  opacity: {}\n", opacity);

        float transparencyFactor = 0.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_TRANSPARENCYFACTOR, &transparencyFactor) == AI_SUCCESS)
            result += std::format("  transparency_factor: {}\n", transparencyFactor);

        float bumpScaling = 0.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_BUMPSCALING, &bumpScaling) == AI_SUCCESS)
            result += std::format("  bump_scaling: {}\n", bumpScaling);

        float shininess = 0.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &shininess) == AI_SUCCESS)
            result += std::format("  shininess: {}\n", shininess);

        float reflectivity = 0.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_REFLECTIVITY, &reflectivity) == AI_SUCCESS)
            result += std::format("  reflectivity: {}\n", reflectivity);

        float shininessStrength = 0.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_SHININESS_STRENGTH, &shininessStrength) == AI_SUCCESS)
            result += std::format("  shininess_strength: {}\n", shininessStrength);

        float refracti = 0.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_REFRACTI, &refracti) == AI_SUCCESS)
            result += std::format("  refraction_index: {}\n", refracti);

        aiColor4D colorDiffuse{};
        if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &colorDiffuse) == AI_SUCCESS)
            result += std::format("  color_diffuse: ({}, {}, {}, {})\n", colorDiffuse.r, colorDiffuse.g, colorDiffuse.b, colorDiffuse.a);

        aiColor4D colorAmbient{};
        if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &colorAmbient) == AI_SUCCESS)
            result += std::format("  color_ambient: ({}, {}, {}, {})\n", colorAmbient.r, colorAmbient.g, colorAmbient.b, colorAmbient.a);

        aiColor4D colorSpecular{};
        if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &colorSpecular) == AI_SUCCESS)
            result += std::format("  color_specular: ({}, {}, {}, {})\n", colorSpecular.r, colorSpecular.g, colorSpecular.b, colorSpecular.a);

        aiColor4D colorEmissive{};
        if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_EMISSIVE, &colorEmissive) == AI_SUCCESS)
            result += std::format("  color_emissive: ({}, {}, {}, {})\n", colorEmissive.r, colorEmissive.g, colorEmissive.b, colorEmissive.a);

        aiColor4D colorTransparent{};
        if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_TRANSPARENT, &colorTransparent) == AI_SUCCESS)
            result += std::format("  color_transparent: ({}, {}, {}, {})\n", colorTransparent.r, colorTransparent.g, colorTransparent.b, colorTransparent.a);

        aiColor4D colorReflective{};
        if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_REFLECTIVE, &colorReflective) == AI_SUCCESS)
            result += std::format("  color_reflective: ({}, {}, {}, {})\n", colorReflective.r, colorReflective.g, colorReflective.b, colorReflective.a);

        aiColor4D baseColor{};
        if (aiGetMaterialColor(mat, AI_MATKEY_BASE_COLOR, &baseColor) == AI_SUCCESS)
            result += std::format("  base_color: ({}, {}, {}, {})\n", baseColor.r, baseColor.g, baseColor.b, baseColor.a);

        float metallicFactor = 0.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_METALLIC_FACTOR, &metallicFactor) == AI_SUCCESS)
            result += std::format("  metallic_factor: {}\n", metallicFactor);

        float roughnessFactor = 0.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_ROUGHNESS_FACTOR, &roughnessFactor) == AI_SUCCESS)
            result += std::format("  roughness_factor: {}\n", roughnessFactor);

        float anisotropyFactor = 0.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_ANISOTROPY_FACTOR, &anisotropyFactor) == AI_SUCCESS)
            result += std::format("  anisotropy_factor: {}\n", anisotropyFactor);

        float specularFactor = 0.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_SPECULAR_FACTOR, &specularFactor) == AI_SUCCESS)
            result += std::format("  specular_factor: {}\n", specularFactor);

        float glossinessFactor = 0.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_GLOSSINESS_FACTOR, &glossinessFactor) == AI_SUCCESS)
            result += std::format("  glossiness_factor: {}\n", glossinessFactor);

        aiColor4D sheenColorFactor{};
        if (aiGetMaterialColor(mat, AI_MATKEY_SHEEN_COLOR_FACTOR, &sheenColorFactor) == AI_SUCCESS)
            result += std::format("  sheen_color_factor: ({}, {}, {}, {})\n", sheenColorFactor.r, sheenColorFactor.g, sheenColorFactor.b, sheenColorFactor.a);

        float sheenRoughnessFactor = 0.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_SHEEN_ROUGHNESS_FACTOR, &sheenRoughnessFactor) == AI_SUCCESS)
            result += std::format("  sheen_roughness_factor: {}\n", sheenRoughnessFactor);

        float clearcoatFactor = 0.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_CLEARCOAT_FACTOR, &clearcoatFactor) == AI_SUCCESS)
            result += std::format("  clearcoat_factor: {}\n", clearcoatFactor);

        float clearcoatRoughnessFactor = 0.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_CLEARCOAT_ROUGHNESS_FACTOR, &clearcoatRoughnessFactor) == AI_SUCCESS)
            result += std::format("  clearcoat_roughness_factor: {}\n", clearcoatRoughnessFactor);

        float transmissionFactor = 0.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_TRANSMISSION_FACTOR, &transmissionFactor) == AI_SUCCESS)
            result += std::format("  transmission_factor: {}\n", transmissionFactor);

        float volumeThicknessFactor = 0.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_VOLUME_THICKNESS_FACTOR, &volumeThicknessFactor) == AI_SUCCESS)
            result += std::format("  volume_thickness_factor: {}\n", volumeThicknessFactor);

        float volumeAttenuationDistance = 0.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_VOLUME_ATTENUATION_DISTANCE, &volumeAttenuationDistance) == AI_SUCCESS)
            result += std::format("  volume_attenuation_distance: {}\n", volumeAttenuationDistance);

        aiColor4D volumeAttenuationColor{};
        if (aiGetMaterialColor(mat, AI_MATKEY_VOLUME_ATTENUATION_COLOR, &volumeAttenuationColor) == AI_SUCCESS)
            result += std::format("  volume_attenuation_color: ({}, {}, {}, {})\n", volumeAttenuationColor.r, volumeAttenuationColor.g, volumeAttenuationColor.b, volumeAttenuationColor.a);

        float emissiveIntensity = 0.0f;
        if (aiGetMaterialFloat(mat, AI_MATKEY_EMISSIVE_INTENSITY, &emissiveIntensity) == AI_SUCCESS)
            result += std::format("  emissive_intensity: {}\n", emissiveIntensity);

        std::vector<std::pair<aiTextureType, const char*>> textureTypes = {
            {aiTextureType_DIFFUSE, "diffuse"},
            {aiTextureType_SPECULAR, "specular"},
            {aiTextureType_AMBIENT, "ambient"},
            {aiTextureType_EMISSIVE, "emissive"},
            {aiTextureType_HEIGHT, "height"},
            {aiTextureType_NORMALS, "normals"},
            {aiTextureType_SHININESS, "shininess"},
            {aiTextureType_OPACITY, "opacity"},
            {aiTextureType_DISPLACEMENT, "displacement"},
            {aiTextureType_LIGHTMAP, "lightmap"},
            {aiTextureType_REFLECTION, "reflection"},
            {aiTextureType_BASE_COLOR, "base_color"},
            {aiTextureType_NORMAL_CAMERA, "normal_camera"},
            {aiTextureType_EMISSION_COLOR, "emission_color"},
            {aiTextureType_METALNESS, "metalness"},
            {aiTextureType_DIFFUSE_ROUGHNESS, "diffuse_roughness"},
            {aiTextureType_AMBIENT_OCCLUSION, "ambient_occlusion"},
            {aiTextureType_SHEEN, "sheen"},
            {aiTextureType_CLEARCOAT, "clearcoat"},
            {aiTextureType_TRANSMISSION, "transmission"},
#ifdef aiTextureType_MAYA_BASE
            {aiTextureType_MAYA_BASE, "maya_base"},
#endif
#ifdef aiTextureType_MAYA_SPECULAR
            {aiTextureType_MAYA_SPECULAR, "maya_specular"},
#endif
#ifdef aiTextureType_MAYA_SPECULAR_COLOR
            {aiTextureType_MAYA_SPECULAR_COLOR, "maya_specular_color"},
#endif
#ifdef aiTextureType_MAYA_SPECULAR_ROUGHNESS
            {aiTextureType_MAYA_SPECULAR_ROUGHNESS, "maya_specular_roughness"},
#endif
#ifdef aiTextureType_ANISOTROPY
            {aiTextureType_ANISOTROPY, "anisotropy"},
#endif
#ifdef aiTextureType_GLTF_METALLIC_ROUGHNESS
            {aiTextureType_GLTF_METALLIC_ROUGHNESS, "gltf_metallic_roughness"},
#endif
            {aiTextureType_UNKNOWN, "unknown"}
        };

        for (const auto& [type, typeName] : textureTypes) {
            unsigned int count = aiGetMaterialTextureCount(mat, type);
            for (unsigned int i = 0; i < count; ++i) {
                aiString path;
                if (aiGetMaterialTexture(mat, type, i, &path) == AI_SUCCESS) {
                    bool isEmbedded = path.data[0] == '*';
                    result += std::format("  texture_{}{}: {} {}\n", typeName, i, path.C_Str(), isEmbedded ? "(embedded)" : "(file)");
                }
            }
        }

        return std::format_to(ctx.out(), "{}", result);
    }
};

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

Mesh AssetLoader::builtinCube(const std::shared_ptr<Material>& material)
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
                .material = material,
            }
        }
    };
    commandBuffer->endBlitPass();
    m_device->submitCommandBuffers(std::move(commandBuffer));
    return mesh;
}

Mesh AssetLoader::loadMesh(const std::filesystem::path& path)
{
    assert(std::filesystem::is_regular_file(path));

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path.string(), POST_PROCESSING_FLAGS);
    if (scene == nullptr)
        throw std::runtime_error("fail to load the model using assimp");

    std::unique_ptr<gfx::CommandBuffer> commandBuffer = m_commandBufferPool->get();
    commandBuffer->beginBlitPass();

    std::map<std::string, std::shared_ptr<gfx::Texture>> textureCache;

    auto materials = std::span(scene->mMaterials, scene->mNumMaterials) | std::views::transform([&](aiMaterial* aiMaterial) -> std::shared_ptr<Material> {
        auto loadTextureFromPath = [&](const aiString& texPath) -> std::shared_ptr<gfx::Texture> {
            auto it = textureCache.find(std::string(texPath.C_Str()));
            if (it != textureCache.end())
                return it->second;
            std::shared_ptr<gfx::Texture> texture;
            if (texPath.data[0] == '*') {
                int texIndex = std::atoi(&texPath.data[1]);
                assert(texIndex >= 0 && texIndex < static_cast<int>(scene->mNumTextures));
                const aiTexture* aiTex = scene->mTextures[texIndex];
                texture = loadEmbeddedTexture(aiTex, commandBuffer.get());
            } else {
                std::filesystem::path texFilePath = path.parent_path() / texPath.C_Str();
                auto texture = loadTexture(texFilePath, commandBuffer.get());
            }
            textureCache[std::string(texPath.C_Str())] = texture;
            return texture;
        };

        auto material = std::make_shared<scop::TexturedMaterial>(*m_device);

        aiColor4D diffuseColor{};
        if (aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor) == AI_SUCCESS)
            material->setDiffuseColor(glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b));
        else
            material->setDiffuseColor(glm::vec3(1.0f, 1.0f, 1.0f));

        aiString diffuseTexturePath;
        if (aiGetMaterialTexture(aiMaterial, aiTextureType_DIFFUSE, 0, &diffuseTexturePath) == AI_SUCCESS)
            material->setDiffuseTexture(loadTextureFromPath(diffuseTexturePath));
        else
            material->setDiffuseTexture(getSolidColorTexture(glm::vec4(1.0f)));

        aiColor4D specularColor{};
        if (aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_SPECULAR, &specularColor) == AI_SUCCESS)
            material->setSpecularColor(glm::vec3(specularColor.r, specularColor.g, specularColor.b));
        else
            material->setSpecularColor(glm::vec3(0.0f, 0.0f, 0.0f));

        float shininess{};
        if (aiGetMaterialFloat(aiMaterial, AI_MATKEY_SHININESS, &shininess) == AI_SUCCESS)
            material->setShininess(shininess);
        else
            material->setShininess(32.0f);

        return material;
    }) | std::ranges::to<std::vector>();


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
            .material = materials[aiMesh->mMaterialIndex],
            .subMeshes = {}
        };
    }) | std::ranges::to<std::vector>();

    commandBuffer->endBlitPass();
    m_device->submitCommandBuffers(std::move(commandBuffer));

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

    return mesh;
}

using UniqueStbiUc = std::unique_ptr<stbi_uc, decltype(&stbi_image_free)>;

std::shared_ptr<gfx::Texture> AssetLoader::loadEmbeddedTexture(const aiTexture* aiTex, gfx::CommandBuffer* commandBuffer)
{
    std::unique_ptr<gfx::CommandBuffer> newCommandBuffer;
    if (commandBuffer == nullptr) {
        newCommandBuffer = m_commandBufferPool->get();
        commandBuffer = newCommandBuffer.get();
    }

    int width = 0;
    int height = 0;
    UniqueStbiUc bytes(nullptr, stbi_image_free);

    if (aiTex->mHeight == 0) {
        bytes.reset(stbi_load_from_memory(std::bit_cast<const stbi_uc*>(aiTex->pcData), static_cast<int>(aiTex->mWidth), &width, &height, nullptr, STBI_rgb_alpha));
    } else {
        width = static_cast<int>(aiTex->mWidth);
        height = static_cast<int>(aiTex->mHeight);
        bytes.reset(static_cast<stbi_uc*>(operator new(sizeof(stbi_uc) * width * height))); // NOLINT(cppcoreguidelines-owning-memory)
        assert(bytes);
        for (int i = 0; i < width * height; ++i) {
            bytes.get()[i * 4 + 0] = aiTex->pcData[i].r;
            bytes.get()[i * 4 + 1] = aiTex->pcData[i].g;
            bytes.get()[i * 4 + 2] = aiTex->pcData[i].b;
            bytes.get()[i * 4 + 3] = aiTex->pcData[i].a;
        }
    }

    if (!bytes)
        throw std::runtime_error("Failed to load embedded texture");

    std::shared_ptr<gfx::Texture> texture = m_device->newTexture(gfx::Texture::Descriptor{
        .type = gfx::TextureType::texture2d,
        .width = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
        .pixelFormat = gfx::PixelFormat::RGBA8Unorm,
        .usages = gfx::TextureUsage::copyDestination | gfx::TextureUsage::shaderRead,
        .storageMode = gfx::ResourceStorageMode::deviceLocal
    });
    assert(texture);

    std::shared_ptr<gfx::Buffer> stagingBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
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
    if (!bytes)
        throw std::runtime_error("failed to load texture: " + path.string());

    std::shared_ptr<gfx::Texture> texture = m_device->newTexture(gfx::Texture::Descriptor{
        .type = gfx::TextureType::texture2d,
        .width = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
        .pixelFormat = gfx::PixelFormat::RGBA8Unorm,
        .usages = gfx::TextureUsage::copyDestination | gfx::TextureUsage::shaderRead,
        .storageMode = gfx::ResourceStorageMode::deviceLocal
    });
    assert(texture);

    std::shared_ptr<gfx::Buffer> stagingBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
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

std::shared_ptr<gfx::Texture> AssetLoader::loadCubeTexture(const std::filesystem::path& right, const std::filesystem::path& left, const std::filesystem::path& top, const std::filesystem::path& bottom, const std::filesystem::path& front, const std::filesystem::path& back, gfx::CommandBuffer* commandBuffer)
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
    std::shared_ptr<gfx::Buffer> stagingBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
        .size = faceSize * 6, // 6 faces
        .usages = gfx::BufferUsage::copySource,
        .storageMode = gfx::ResourceStorageMode::hostVisible
    });
    assert(stagingBuffer);

    auto* bufferData = stagingBuffer->content<stbi_uc>();
    std::memcpy(bufferData + 0 * faceSize, bytes.at(right).get(), faceSize);  // +X (right)
    std::memcpy(bufferData + 1 * faceSize, bytes.at(left).get(), faceSize);   // -X (left)
    std::memcpy(bufferData + 2 * faceSize, bytes.at(top).get(), faceSize);    // +Y (top)
    std::memcpy(bufferData + 3 * faceSize, bytes.at(bottom).get(), faceSize); // -Y (bottom)
    std::memcpy(bufferData + 4 * faceSize, bytes.at(front).get(), faceSize);  // +Z (front)
    std::memcpy(bufferData + 5 * faceSize, bytes.at(back).get(), faceSize);   // -Z (back)

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

std::shared_ptr<gfx::Texture> AssetLoader::getSolidColorTexture(const glm::vec4& color, gfx::CommandBuffer* commandBuffer)
{
    auto it =std::ranges::find_if(m_solidColorTextureCache, [&](const auto& e){ return e.first == color; });
    if (it != m_solidColorTextureCache.end())
        return it->second;

    std::unique_ptr<gfx::CommandBuffer> newCommandBuffer;
    if (commandBuffer == nullptr) {
        newCommandBuffer = m_commandBufferPool->get();
        commandBuffer = newCommandBuffer.get();
    }

    std::shared_ptr<gfx::Texture> texture = m_device->newTexture(gfx::Texture::Descriptor{
        .type = gfx::TextureType::texture2d,
        .width = 1, .height = 1,
        .pixelFormat = gfx::PixelFormat::RGBA8Unorm,
        .usages = gfx::TextureUsage::copyDestination | gfx::TextureUsage::shaderRead,
        .storageMode = gfx::ResourceStorageMode::deviceLocal
    });
    assert(texture);

    std::shared_ptr<gfx::Buffer> stagingBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
        .size = 4, // 1 pixel * 4 bytes (RGBA)
        .usages = gfx::BufferUsage::copySource,
        .storageMode = gfx::ResourceStorageMode::hostVisible
    });
    assert(stagingBuffer);

    auto* pixelData = stagingBuffer->content<uint8_t>();
    pixelData[0] = static_cast<uint8_t>(color.r * 255.0f);
    pixelData[1] = static_cast<uint8_t>(color.g * 255.0f);
    pixelData[2] = static_cast<uint8_t>(color.b * 255.0f);
    pixelData[3] = static_cast<uint8_t>(color.a * 255.0f);

    if (newCommandBuffer != nullptr)
        newCommandBuffer->beginBlitPass();

    commandBuffer->copyBufferToTexture(stagingBuffer, texture);

    if (newCommandBuffer != nullptr) {
        newCommandBuffer->endBlitPass();
        m_device->submitCommandBuffers(std::move(newCommandBuffer));
    }

    m_solidColorTextureCache.emplace_back(color, texture);

    return texture;
}

}
