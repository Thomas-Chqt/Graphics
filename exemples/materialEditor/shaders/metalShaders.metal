/*
 * ---------------------------------------------------
 * metalShaders.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/07 09:27:04
 * ---------------------------------------------------
 */

#include <metal_stdlib>

using namespace metal;

struct VertexIn
{
    float3 pos    [[attribute(0)]];
    float2 uv     [[attribute(1)]];
    float3 normal [[attribute(2)]];
};

struct VertexOut
{
    float3 pos;
    float4 clipPos [[position]];
    float2 uv;
    float3 normal;
};

struct Matrices
{
    float4x4 vpMatrix;
    float4x4 modelMatrix;
};

vertex VertexOut universal3D(VertexIn in [[stage_in]], constant Matrices& matrices [[buffer(1)]])
{
    float4 worldPos = matrices.modelMatrix * float4(in.pos, 1.0);
    return (VertexOut){
        .pos     = worldPos.xyz,
        .clipPos = matrices.vpMatrix * worldPos,
        .uv      = in.uv,
        .normal  = (matrices.modelMatrix * float4(in.normal, 0)).xyz
    };
}

struct Material
{
    float3 diffuseColor;
    float3 specularColor;
    float3 emissiveColor;
    float  shininess;

    bool useDiffuseTexture;
};

struct DirectionalLight
{
    float3 color;
    float  ambiantIntensity;
    float  diffuseIntensity;
    float  specularIntensity;
    float3 direction;
};

fragment float4 phong1(VertexOut in  [[stage_in]],
    constant Material& material      [[buffer(0)]],
    constant DirectionalLight& light [[buffer(1)]],
    texture2d<float> diffuseTexture  [[texture(1)]]
)
{
    constexpr metal::sampler diffuseTextureSampler(metal::mag_filter::nearest, metal::min_filter::nearest);

    float3 diffuseColor = material.diffuseColor;
    if (material.useDiffuseTexture)
        diffuseColor = diffuseTexture.sample(diffuseTextureSampler, in.uv).xyz;

    float3 cameraDir = normalize(float3(0, 0, -3) - in.pos);

    float diffuseFactor = dot(normalize(in.normal), normalize(-light.direction));
    float specularFactor = dot(reflect(light.direction, in.normal), cameraDir);

    float3 ambiant  = diffuseColor           * light.color * light.ambiantIntensity;
    float3 diffuse  = diffuseColor           * light.color * light.diffuseIntensity  * max(diffuseFactor, 0.0F);
    float3 specular = material.specularColor * light.color * light.specularIntensity * (specularFactor > 0.0f ? pow(specularFactor, material.shininess) : 0.0f);
    float3 emissive = material.emissiveColor;

    return float4(ambiant + diffuse + specular + emissive, 1.0F);
}