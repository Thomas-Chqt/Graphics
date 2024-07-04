/*
 * ---------------------------------------------------
 * metalShaders.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/21 17:57:43
 * ---------------------------------------------------
 */

#include <metal_stdlib>
#include "ShaderStructs.hpp"

using namespace metal;

struct VertexOut
{
    float3 pos;
    float4 clipPos [[position]];
    float2 uv;
    float3 normal;
};

vertex VertexOut universal3D( uint vID [[vertex_id]],
    constant Vertex* vertices        [[buffer(0)]],
    constant float4x4& u_modelMatrix [[buffer(1)]],
    constant float4x4& u_vpMatrix    [[buffer(2)]]
)
{
    return (VertexOut){
        .pos     = (u_modelMatrix * float4(vertices[vID].pos, 1.0)).xyz,
        .clipPos = u_vpMatrix * u_modelMatrix * float4(vertices[vID].pos, 1.0),
        .uv      = vertices[vID].uv,
        .normal  = (u_modelMatrix * float4(vertices[vID].normal, 0)).xyz
    };
}

struct Fragment
{
    float3 position;
    float3 normal;
    float3 ambiant;
    float3 diffuse;
    float3 specular;
    float3 emissive;
    float  shininess;
};

float3 computePointLight(constant PointLight& light, const thread Fragment& frag, float3 cameraDir)
{
    float3 lightDir = normalize(light.position - frag.position);

    float diffuseFactor  = dot(frag.normal, lightDir);
    float specularFactor = dot(reflect(-lightDir, frag.normal), cameraDir);

    float3 ambiant  = frag.ambiant  * light.color * light.ambiantIntensity;
    float3 diffuse  = frag.diffuse  * light.color * light.diffuseIntensity * max(diffuseFactor, 0.0f);
    float3 specular = frag.specular * light.color * light.specularIntensity * (specularFactor > 0.0f ? pow(specularFactor, frag.shininess) : 0.0f);
    float3 emissive = frag.emissive;

    return ambiant + diffuse + specular + emissive;
}

fragment float4 universal_fs(VertexOut in [[stage_in]],
    constant float3& u_cameraPos             [[buffer(1)]],
    constant PointLight* u_pointLights       [[buffer(2)]],
    constant uint& u_pointLightsCount        [[buffer(3)]],
    constant universal::Material& u_material [[buffer(4)]],
    texture2d<float> u_ambientTexture        [[texture(0)]],
    texture2d<float> u_diffuseTexture        [[texture(1)]],
    texture2d<float> u_specularTexture       [[texture(2)]],
    texture2d<float> u_emissiveTexture       [[texture(3)]],
    texture2d<float> u_shininessTexture      [[texture(4)]]
)
{
    constexpr metal::sampler textureSampler(metal::mag_filter::nearest, metal::min_filter::nearest);

    float4 ambientBase;
    if (u_material.useAmbientTexture != 0)
        ambientBase = u_ambientTexture.sample(textureSampler, in.uv);
    else
        ambientBase = float4(u_material.ambientColor, 1.0f);

    float4 diffuseBase;
    if (u_material.useDiffuseTexture != 0)
        diffuseBase = u_diffuseTexture.sample(textureSampler, in.uv);
    else
        diffuseBase = float4(u_material.diffuseColor, 1.0f);

    float4 specularBase;
    if (u_material.useSpecularTexture != 0)
        specularBase = u_specularTexture.sample(textureSampler, in.uv);
    else
        specularBase = float4(u_material.specularColor, 1.0f);

    float4 emissiveBase;
    if (u_material.useEmissiveTexture != 0)
        emissiveBase = u_emissiveTexture.sample(textureSampler, in.uv);
    else
        emissiveBase = float4(u_material.emissiveColor, 1.0f);

    float shininess;
    // if (u_material.useShininessTexture != 0)
    //     shininess = u_shininessTexture.sample(textureSampler, in.uv);
    // else
        shininess = u_material.shininess;

    Fragment fragInfos = {
        .position  = in.pos,
        .normal    = normalize(in.normal),
        .ambiant   = ambientBase.xyz,
        .diffuse   = diffuseBase.xyz,
        .specular  = specularBase.xyz,
        .emissive  = emissiveBase.xyz,
        .shininess = shininess
    };

    float3 cameraDir = normalize(u_cameraPos - in.pos);

    float3 output = float3(0.0f, 0.0f, 0.0f);
    for(uint i = 0; i < u_pointLightsCount; i++)
        output += computePointLight(u_pointLights[i], fragInfos, cameraDir);

    return float4(output, diffuseBase.w);
}

fragment float4 baseColor_fs(VertexOut in [[stage_in]],
    constant float3& u_cameraPos             [[buffer(1)]],
    constant PointLight* u_pointLights       [[buffer(2)]],
    constant uint& u_pointLightsCount        [[buffer(3)]],
    constant baseColor::Material& u_material [[buffer(4)]]
)
{
    Fragment fragInfos = {
        .position  = in.pos,
        .normal    = normalize(in.normal),
        .ambiant   = u_material.baseColor,
        .diffuse   = u_material.baseColor,
        .specular  = u_material.specularColor,
        .emissive  = u_material.emissiveColor,
        .shininess = u_material.shininess
    };

    float3 cameraDir = normalize(u_cameraPos - in.pos);

    float3 output = float3(0.0f, 0.0f, 0.0f);
    for(uint i = 0; i < u_pointLightsCount; i++)
        output += computePointLight(u_pointLights[i], fragInfos, cameraDir);

    return float4(output, 1.0f);
}

fragment float4 baseTexture_fs(VertexOut in [[stage_in]],
    constant float3& u_cameraPos               [[buffer(1)]],
    constant PointLight* u_pointLights         [[buffer(2)]],
    constant uint& u_pointLightsCount          [[buffer(3)]],
    constant baseTexture::Material& u_material [[buffer(4)]],
    texture2d<float> u_diffuseTexture          [[texture(0)]]
)
{
    constexpr metal::sampler textureSampler(metal::mag_filter::nearest, metal::min_filter::nearest);

    float4 baseColor = u_diffuseTexture.sample(textureSampler, in.uv);

    Fragment fragInfos = {
        .position  = in.pos,
        .normal    = normalize(in.normal),
        .ambiant   = baseColor.xyz,
        .diffuse   = baseColor.xyz,
        .specular  = u_material.specularColor,
        .emissive  = u_material.emissiveColor,
        .shininess = u_material.shininess
    };

    float3 cameraDir = normalize(u_cameraPos - in.pos);

    float3 output = float3(0.0f, 0.0f, 0.0f);
    for(uint i = 0; i < u_pointLightsCount; i++)
        output += computePointLight(u_pointLights[i], fragInfos, cameraDir);

    return float4(output, baseColor.w);
}