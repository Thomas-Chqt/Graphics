/*
 * ---------------------------------------------------
 * AssetLoader.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/21 18:53:38
 * ---------------------------------------------------
 */

#include "AssetLoader.hpp"
#include "Graphics/Enums.hpp"
#include "Mesh.hpp"
#include "Material.hpp"

#include <Graphics/Device.hpp>
#include <Graphics/CommandBuffer.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Buffer.hpp>

#if !defined (SCOP_MANDATORY)
    #include <glm/glm.hpp>
    #include <assimp/Importer.hpp>
    #include <assimp/postprocess.h>
    #include <assimp/scene.h>
    #include <assimp/types.h>
    #include <glm/ext/matrix_transform.hpp>
    #include <tracy/Tracy.hpp>
    #include <tracy/TracyC.h>
#else
    #include "math/math.hpp"
    #include "ObjParser/ObjParser.hpp"
    #ifndef SCOP_MATH_GLM_ALIAS_DEFINED
        #define SCOP_MATH_GLM_ALIAS_DEFINED
        namespace glm = scop::math;
    #endif
#endif
#include <stb_image/stb_image.h>

#include <algorithm>
#include <bit> // IWYU pragma: keep
#include <cstdlib>
#include <format> // IWYU pragma: keep
#include <functional> // IWYU pragma: keep
#include <span> // IWYU pragma: keep
#include <array>
#include <cstdint>
#include <map>
#include <ranges> // IWYU pragma: keep
#include <cstring>
#include <filesystem>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <mutex>
#include <cmath>

#if !defined (SCOP_MANDATORY)
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
#endif

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

#if !defined (SCOP_MANDATORY)
constexpr unsigned int POST_PROCESSING_FLAGS = aiProcess_CalcTangentSpace      |
                                               aiProcess_JoinIdenticalVertices |
                                               aiProcess_Triangulate           |
                                               aiProcess_GenNormals            |
                                               aiProcess_OptimizeMeshes        |
                                               aiProcess_FlipUVs;

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
#endif

AssetLoader::AssetLoader(gfx::Device* device)
    : m_device(device)
{
    assert(m_device);
}

Mesh AssetLoader::builtinCube(const std::shared_ptr<Material>& material)
{
    std::unique_ptr<gfx::CommandBufferPool> commandBufferPool = m_device->newCommandBufferPool();
    assert(commandBufferPool);

    std::shared_ptr<gfx::CommandBuffer> commandBuffer = commandBufferPool->get();
    auto blitPassDescriptor = m_device->newBlitPassDescriptor();
    commandBuffer->beginBlitPass(*blitPassDescriptor);
    auto mesh = Mesh{
        .name = "cube_mesh",
        .bBoxMin = {-0.5, -0.5, -0.5},
        .bBoxMax = { 0.5,  0.5,  0.5},
        .subMeshes = {
            SubMesh{
                .name = "cube_submesh",
                .transform = glm::mat4x4(1),
                .vertexBuffer = newVertexBuffer(cube_vertices, *commandBuffer),
                .indexBuffer = newIndexBuffer(cube_indices, *commandBuffer),
                .material = material ? material : std::make_shared<FlatColorMaterial>(*m_device),
            }
        }
    };
    commandBuffer->endBlitPass();
    m_device->submitCommandBuffers(commandBuffer);
    return mesh;
}

#if !defined (SCOP_MANDATORY)
Mesh AssetLoader::loadMesh(const std::filesystem::path& path, std::optional<std::shared_ptr<Material>> overrideMaterial)
{
    ZoneScoped;
    assert(std::filesystem::is_regular_file(path));

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path.string(), POST_PROCESSING_FLAGS);
    if (scene == nullptr)
        throw std::runtime_error("fail to load the model using assimp");

    std::unique_ptr<gfx::CommandBufferPool> commandBufferPool = m_device->newCommandBufferPool();
    assert(commandBufferPool);

    std::shared_ptr<gfx::CommandBuffer> commandBuffer = commandBufferPool->get();
    auto blitPassDescriptor = m_device->newBlitPassDescriptor();
    commandBuffer->beginBlitPass(*blitPassDescriptor);

    std::vector<std::shared_ptr<Material>> materials;
    if (overrideMaterial.has_value()) {
        materials.assign(scene->mNumMaterials, *overrideMaterial);
    } else {
        std::unique_ptr<gfx::ParameterBlockPool> parameterBlockPool = m_device->newParameterBlockPool({
            .maxBindingCount = {
                {gfx::BindingType::constantBuffer, 500},
                {gfx::BindingType::sampledTexture, 3500},
                {gfx::BindingType::sampler, 1000}
            }
        });
        assert(parameterBlockPool);

        std::map<std::pair<std::string, gfx::PixelFormat>, std::shared_ptr<gfx::Texture>> textureCache;

        auto loadTextureFromPath = [&](const aiString& texPath, gfx::PixelFormat pixelFormat) -> std::shared_ptr<gfx::Texture> {
            const auto cacheKey = std::make_pair(std::string(texPath.C_Str()), pixelFormat);
            auto it = textureCache.find(cacheKey);
            if (it != textureCache.end())
                return it->second;
            std::shared_ptr<gfx::Texture> texture;
            if (texPath.data[0] == '*') {
                int texIndex = std::atoi(&texPath.data[1]);
                assert(texIndex >= 0 && texIndex < static_cast<int>(scene->mNumTextures));
                const aiTexture* aiTex = scene->mTextures[texIndex];
                texture = loadEmbeddedTexture(aiTex, *commandBuffer, pixelFormat);
            } else {
                std::filesystem::path texFilePath = path.parent_path() / texPath.C_Str();
                texture = loadTexture(texFilePath, *commandBuffer, pixelFormat);
            }
            textureCache[cacheKey] = texture;
            return texture;
        };

        materials = std::span(scene->mMaterials, scene->mNumMaterials) | std::views::transform([&](aiMaterial* aiMaterial) -> std::shared_ptr<Material> {
            ZoneScopedN("makeMaterial");

            auto material = std::make_shared<scop::PbrMaterial>(*m_device);

            aiColor4D baseColor{};
            if (aiGetMaterialColor(aiMaterial, AI_MATKEY_BASE_COLOR, &baseColor) == AI_SUCCESS || aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_DIFFUSE, &baseColor) == AI_SUCCESS)
                material->setBaseColor(glm::vec4(baseColor.r, baseColor.g, baseColor.b, baseColor.a));

            aiString baseColorTexturePath;
            if (aiGetMaterialTexture(aiMaterial, aiTextureType_BASE_COLOR, 0, &baseColorTexturePath) == AI_SUCCESS || aiGetMaterialTexture(aiMaterial, aiTextureType_DIFFUSE, 0, &baseColorTexturePath) == AI_SUCCESS)
                material->setBaseColorTexture(loadTextureFromPath(baseColorTexturePath, gfx::PixelFormat::RGBA8_sRGB));
            else
                material->setBaseColorTexture(getSolidColorTexture(glm::vec4(1.0f), *commandBuffer));

            float metallic = 0.0f;
            if (aiGetMaterialFloat(aiMaterial, AI_MATKEY_METALLIC_FACTOR, &metallic) == AI_SUCCESS)
                material->setMetallic(std::clamp(metallic, 0.0f, 1.0f));

            float roughness = 1.0f;
            if (aiGetMaterialFloat(aiMaterial, AI_MATKEY_ROUGHNESS_FACTOR, &roughness) == AI_SUCCESS)
                material->setRoughness(std::clamp(roughness, 0.01f, 1.0f));

            aiString metallicRoughnessTexturePath;
            if (aiGetMaterialTexture(aiMaterial, aiTextureType_GLTF_METALLIC_ROUGHNESS, 0, &metallicRoughnessTexturePath) == AI_SUCCESS)
            {
                const std::shared_ptr<gfx::Texture> metallicRoughnessTexture = loadTextureFromPath(
                    metallicRoughnessTexturePath,
                    gfx::PixelFormat::RGBA8_unorm
                );
                material->setMetallicTexture(metallicRoughnessTexture);
                material->setRoughnessTexture(metallicRoughnessTexture);
                material->setMetallicTextureChannel(2);
                material->setRoughnessTextureChannel(1);
            }
            else
            {
                aiString metallicTexturePath;
                if (aiGetMaterialTexture(aiMaterial, aiTextureType_METALNESS, 0, &metallicTexturePath) == AI_SUCCESS)
                    material->setMetallicTexture(loadTextureFromPath(metallicTexturePath, gfx::PixelFormat::RGBA8_unorm));
                else
                    material->setMetallicTexture(getSolidColorTexture(glm::vec4(1.0f), *commandBuffer));

                aiString roughnessTexturePath;
                if (aiGetMaterialTexture(aiMaterial, aiTextureType_DIFFUSE_ROUGHNESS, 0, &roughnessTexturePath) == AI_SUCCESS)
                    material->setRoughnessTexture(loadTextureFromPath(roughnessTexturePath, gfx::PixelFormat::RGBA8_unorm));
                else
                    material->setRoughnessTexture(getSolidColorTexture(glm::vec4(1.0f), *commandBuffer));
            }

            aiString normalTexturePath;
            if (aiGetMaterialTexture(aiMaterial, aiTextureType_NORMALS, 0, &normalTexturePath) == AI_SUCCESS) {
                material->setNormalTexture(loadTextureFromPath(normalTexturePath, gfx::PixelFormat::RGBA8_unorm));
            } else {
                material->setNormalTexture(getSolidColorTexture(glm::vec4(0.5f, 0.5f, 1.0f, 1.0f), *commandBuffer)); // Neutral normal: (128, 128, 255) = (0, 0, 1) in tangent space
            }

            glm::vec3 emissiveFactor(0.0f);
            aiColor4D emissiveColor{};
            if (aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_EMISSIVE, &emissiveColor) == AI_SUCCESS)
                emissiveFactor = glm::vec3(emissiveColor.r, emissiveColor.g, emissiveColor.b);
            float emissiveIntensity = 1.0f;
            if (aiGetMaterialFloat(aiMaterial, AI_MATKEY_EMISSIVE_INTENSITY, &emissiveIntensity) == AI_SUCCESS)
                emissiveFactor *= emissiveIntensity;
            material->setEmissiveFactor(emissiveFactor);

            aiString emissiveTexturePath;
            if (aiGetMaterialTexture(aiMaterial, aiTextureType_EMISSION_COLOR, 0, &emissiveTexturePath) == AI_SUCCESS || aiGetMaterialTexture(aiMaterial, aiTextureType_EMISSIVE, 0, &emissiveTexturePath) == AI_SUCCESS)
                material->setEmissiveTexture(loadTextureFromPath(emissiveTexturePath, gfx::PixelFormat::RGBA8_sRGB));
            else
                material->setEmissiveTexture(getSolidColorTexture(glm::vec4(1.0f), *commandBuffer));

            material->makeParameterBlock(*parameterBlockPool);

            return material;
        }) | std::ranges::to<std::vector>();
    }

    auto flatSubMeshes = std::span(scene->mMeshes, scene->mNumMeshes) | std::views::transform([&](aiMesh* aiMesh) -> SubMesh{
        return SubMesh{
            .name = aiMesh->mName.C_Str(),
            .transform = glm::mat4x4(1.0f),
            .vertexBuffer = newVertexBuffer(std::views::iota(0u, aiMesh->mNumVertices) | std::views::transform([aiMesh](uint32_t i) -> Vertex{
                glm::vec4 tangent(0.0f, 0.0f, 0.0f, 1.0f);
                if (aiMesh->mNormals != nullptr && aiMesh->mTangents != nullptr && aiMesh->mBitangents != nullptr)
                {
                    const aiVector3D& normal = aiMesh->mNormals[i];
                    const aiVector3D& aiTangent = aiMesh->mTangents[i];
                    const aiVector3D& bitangent = aiMesh->mBitangents[i];
                    const float handedness = ((normal ^ aiTangent) * bitangent) < 0.0f ? -1.0f : 1.0f;
                    tangent = glm::vec4(aiTangent.x, aiTangent.y, aiTangent.z, handedness);
                }

                return Vertex{
                    .pos = glm::vec3(aiMesh->mVertices[i].x, aiMesh->mVertices[i].y, aiMesh->mVertices[i].z),
                    .uv = aiMesh->mTextureCoords[0] != nullptr ? glm::vec2(aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y) : glm::vec2(0.0f),
                    .normal = aiMesh->mNormals != nullptr ? glm::vec3(aiMesh->mNormals[i].x, aiMesh->mNormals[i].y, aiMesh->mNormals[i].z) : glm::vec3(0.0f),
                    .tangent = tangent
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
    m_device->submitCommandBuffers(commandBuffer);

    std::function<void(std::vector<SubMesh>&, aiNode*, glm::mat4x4)> addNode = [&](std::vector<SubMesh>& dest, aiNode* aiNode, glm::mat4x4 additionalTransform) {
        glm::mat4x4 transform = additionalTransform * toGlmMat4(aiNode->mTransformation);

        std::vector<SubMesh> subMeshes = std::span(aiNode->mMeshes, aiNode->mNumMeshes) | std::views::transform([&](uint32_t i) -> SubMesh {
            SubMesh submesh = flatSubMeshes[i];
            submesh.transform = transform;
            return submesh;
        }) | std::ranges::to<std::vector>();

        for (auto* node : std::span(aiNode->mChildren, aiNode->mNumChildren)) {
            if (subMeshes.empty())
                addNode(dest, node, transform);
            else
                addNode(subMeshes.front().subMeshes, node, glm::mat4x4(1.0F));
        }

        #ifdef __cpp_lib_containers_ranges
        dest.append_range(std::move(subMeshes));
        #else
        dest.insert(dest.end(), subMeshes.cbegin(), subMeshes.cend());
        #endif
    };

    Mesh mesh = { .name = scene->mRootNode->mName.C_Str() };
    addNode(mesh.subMeshes, scene->mRootNode, glm::mat4x4(1.0F));

    return mesh;
}
#else
Mesh AssetLoader::loadMesh(const std::filesystem::path& path, std::optional<std::shared_ptr<Material>> overrideMaterial)
{
    assert(std::filesystem::is_regular_file(path));

    ObjMesh meshData;
    try
    {
        meshData = parseObjFile(path);
    }
    catch (...)
    {
        throw std::runtime_error("unable to parse the model file");
    }
    auto& positions = meshData.positions;
    const auto& indices = meshData.indices;
    const auto& normals = meshData.normals;

    if (positions.size() == 0)
        throw std::runtime_error("unable to parse the model file");
    if (normals && normals->size() != positions.size())
        throw std::runtime_error("unable to parse the model file");

    auto [minXIt, maxXIt] = std::ranges::minmax_element(positions, {}, &glm::vec3::x);
    auto [minYIt, maxYIt] = std::ranges::minmax_element(positions, {}, &glm::vec3::y);
    auto [minZIt, maxZIt] = std::ranges::minmax_element(positions, {}, &glm::vec3::z);

    glm::vec3 bBoxMin{ minXIt->x, minYIt->y, minZIt->z };
    glm::vec3 bBoxMax{ maxXIt->x, maxYIt->y, maxZIt->z };

    const glm::vec3 center = (bBoxMin + bBoxMax) * 0.5f;
    bBoxMin -= center;
    bBoxMax -= center;

    for (auto& pos : positions)
        pos -= center;

    std::unique_ptr<gfx::CommandBufferPool> commandBufferPool = m_device->newCommandBufferPool();
    assert(commandBufferPool);

    std::shared_ptr<gfx::CommandBuffer> commandBuffer = commandBufferPool->get();
    auto blitPassDescriptor = m_device->newBlitPassDescriptor();
    commandBuffer->beginBlitPass(*blitPassDescriptor);

    std::shared_ptr<Material> material;
    if (overrideMaterial.has_value()) {
        material = *overrideMaterial;
    } else {
        std::shared_ptr<gfx::ParameterBlockPool> parameterBlockPool = m_device->newParameterBlockPool({
            .maxBindingCount = {
                { gfx::BindingType::constantBuffer, 1 },
                { gfx::BindingType::sampledTexture, 8 },
                { gfx::BindingType::sampler, 2 }
            }
        });
        assert(parameterBlockPool);
        auto pbrMaterial = std::make_shared<PbrMaterial>(*m_device);
        pbrMaterial->setBaseColorTexture(getSolidColorTexture(glm::vec4(1.0f), *commandBuffer));
        pbrMaterial->setNormalTexture(getSolidColorTexture(glm::vec4(0.5f, 0.5f, 1.0f, 1.0f), *commandBuffer));
        pbrMaterial->setMetallicTexture(getSolidColorTexture(glm::vec4(1.0f), *commandBuffer));
        pbrMaterial->setRoughnessTexture(getSolidColorTexture(glm::vec4(1.0f), *commandBuffer));
        pbrMaterial->setEmissiveTexture(getSolidColorTexture(glm::vec4(1.0f), *commandBuffer));
        pbrMaterial->makeParameterBlock(*parameterBlockPool);
        material = std::move(pbrMaterial);
    }

    Mesh mesh = {
        .name = path.stem().string(),
        .bBoxMin = bBoxMin,
        .bBoxMax = bBoxMax,
        .subMeshes = {
            SubMesh{
                .name = path.stem().string(),
                .transform = glm::mat4x4(1.0f),
                .vertexBuffer = newVertexBuffer(std::views::iota(0u, positions.size())
                    | std::views::transform([&](uint32_t i) -> Vertex {
                        return Vertex{
                            .pos = positions[i],
                            .normal = normals ? (*normals)[i] : glm::vec3(0.0f)
                        };
                    })
                    | std::ranges::to<std::vector>(), *commandBuffer),
                .indexBuffer = newIndexBuffer(indices, *commandBuffer),
                .material = material,
            }
        }
    };

    commandBuffer->endBlitPass();
    m_device->submitCommandBuffers(commandBuffer);

    return mesh;
}
#endif

using UniqueStbiUc = std::unique_ptr<stbi_uc, decltype(&stbi_image_free)>;

#if !defined (SCOP_MANDATORY)
std::shared_ptr<gfx::Texture> AssetLoader::loadEmbeddedTexture(const aiTexture* aiTex, gfx::CommandBuffer& commandBuffer, gfx::PixelFormat pixelFormat )
{
    ZoneScoped;
    assert(pixelFormat == gfx::PixelFormat::RGBA8_unorm || pixelFormat == gfx::PixelFormat::RGBA8_sRGB);

    int width = 0;
    int height = 0;
    UniqueStbiUc bytes(nullptr, stbi_image_free);

    if (aiTex->mHeight == 0) {
        bytes.reset(stbi_load_from_memory(std::bit_cast<const stbi_uc*>(aiTex->pcData), static_cast<int>(aiTex->mWidth), &width, &height, nullptr, STBI_rgb_alpha));
    } else {
        width = static_cast<int>(aiTex->mWidth);
        height = static_cast<int>(aiTex->mHeight);
        bytes.reset(static_cast<stbi_uc*>(std::malloc(sizeof(stbi_uc) * width * height * 4))); // NOLINT(cppcoreguidelines-owning-memory,cppcoreguidelines-no-malloc)
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
        .mipLevelCount = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1,
        .pixelFormat = pixelFormat,
        .usages = gfx::TextureUsage::copyDestination | gfx::TextureUsage::shaderRead | gfx::TextureUsage::copySource,
        .storageMode = gfx::ResourceStorageMode::deviceLocal
    });
    assert(texture);

    std::shared_ptr<gfx::Buffer> stagingBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
        .size = static_cast<size_t>(width) * static_cast<size_t>(height) * pixelFormatSize(pixelFormat),
        .usages = gfx::BufferUsage::copySource,
        .storageMode = gfx::ResourceStorageMode::hostVisible
    });
    assert(stagingBuffer);

    std::memcpy(stagingBuffer->content<stbi_uc>(), bytes.get(), stagingBuffer->size());

    commandBuffer.copyBufferToTexture(stagingBuffer, texture);
    if (texture->mipLevelCount() > 1)
        commandBuffer.generateMipmaps(texture);

    return texture;
}
#endif

std::shared_ptr<gfx::Texture> AssetLoader::loadTexture(const std::filesystem::path& path, gfx::CommandBuffer& commandBuffer, gfx::PixelFormat pixelFormat)
{
    ZoneScoped;
    assert(pixelFormat == gfx::PixelFormat::RGBA8_unorm || pixelFormat == gfx::PixelFormat::RGBA8_sRGB);

    int width = 0;
    int height = 0;
    UniqueStbiUc bytes = UniqueStbiUc(stbi_load(path.string().c_str(), &width, &height, nullptr, STBI_rgb_alpha), stbi_image_free);
    if (!bytes)
        throw std::runtime_error("failed to load texture: " + path.string());

    std::shared_ptr<gfx::Texture> texture = m_device->newTexture(gfx::Texture::Descriptor{
        .type = gfx::TextureType::texture2d,
        .width = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
        .mipLevelCount = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1,
        .pixelFormat = pixelFormat,
        .usages = gfx::TextureUsage::copyDestination | gfx::TextureUsage::shaderRead | gfx::TextureUsage::copySource,
        .storageMode = gfx::ResourceStorageMode::deviceLocal
    });
    assert(texture);

    std::shared_ptr<gfx::Buffer> stagingBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
        .size = static_cast<size_t>(width) * static_cast<size_t>(height) * pixelFormatSize(pixelFormat),
        .usages = gfx::BufferUsage::copySource,
        .storageMode = gfx::ResourceStorageMode::hostVisible
    });
    assert(stagingBuffer);

    std::memcpy(stagingBuffer->content<stbi_uc>(), bytes.get(), stagingBuffer->size());

    commandBuffer.copyBufferToTexture(stagingBuffer, texture);
    if (texture->mipLevelCount() > 1)
        commandBuffer.generateMipmaps(texture);

    return texture;
}

std::shared_ptr<gfx::Texture> AssetLoader::loadCubeTexture(const std::filesystem::path& right, const std::filesystem::path& left, const std::filesystem::path& top, const std::filesystem::path& bottom, const std::filesystem::path& front, const std::filesystem::path& back, gfx::CommandBuffer& commandBuffer)
{
    ZoneScoped;

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
        .pixelFormat = gfx::PixelFormat::RGBA8_unorm,
        .usages = gfx::TextureUsage::copyDestination | gfx::TextureUsage::shaderRead,
        .storageMode = gfx::ResourceStorageMode::deviceLocal
    });
    assert(texture);

    size_t faceSize = static_cast<size_t>(width) * static_cast<size_t>(height) * pixelFormatSize(gfx::PixelFormat::RGBA8_unorm);
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

    for (int face = 0; face < 6; ++face)
        commandBuffer.copyBufferToTexture(stagingBuffer, face * faceSize, texture, face);

    return texture;
}

std::shared_ptr<gfx::Texture> AssetLoader::getSolidColorTexture(const glm::vec4& color, gfx::CommandBuffer& commandBuffer)
{
    ZoneScoped;

    std::scoped_lock lock(m_solidColorTextureCacheMtx);

    auto it =std::ranges::find_if(m_solidColorTextureCache, [&](const auto& e){ return e.first == color; });
    if (it != m_solidColorTextureCache.end())
        return it->second;

    std::shared_ptr<gfx::Texture> texture = m_device->newTexture(gfx::Texture::Descriptor{
        .type = gfx::TextureType::texture2d,
        .width = 1, .height = 1,
        .pixelFormat = gfx::PixelFormat::RGBA8_unorm,
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
    pixelData[0] = static_cast<uint8_t>(color.x * 255.0f);
    pixelData[1] = static_cast<uint8_t>(color.y * 255.0f);
    pixelData[2] = static_cast<uint8_t>(color.z * 255.0f);
    pixelData[3] = static_cast<uint8_t>(color.w * 255.0f);

    commandBuffer.copyBufferToTexture(stagingBuffer, texture);

    m_solidColorTextureCache.emplace_back(color, texture);

    return texture;
}

}
