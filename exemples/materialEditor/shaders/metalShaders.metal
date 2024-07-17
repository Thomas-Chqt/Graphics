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
    float3 pos     [[attribute(0)]];
    float2 uv      [[attribute(1)]];
    float3 normal  [[attribute(2)]];
    float3 tangent [[attribute(3)]];
};

struct VertexOut
{
    float3   pos;
    float4   clipPos [[position]];
    float2   uv;
    float3   tangent;
    float3   bitangent;
    float3   normal;
};

struct Matrices
{
    float4x4 vpMatrix;
    float4x4 modelMatrix;
};

vertex VertexOut universal3D(VertexIn in [[stage_in]], constant Matrices& matrices [[buffer(1)]])
{
    float4 worldPos  = matrices.modelMatrix * float4(in.pos, 1.0);
    float3 bitangent = cross(in.tangent, in.normal);

    return (VertexOut){
        .pos       = worldPos.xyz,
        .clipPos   = matrices.vpMatrix * worldPos,
        .uv        = in.uv,
        .tangent   = (matrices.modelMatrix * float4(in.tangent, 0)).xyz,
        .bitangent = (matrices.modelMatrix * float4(bitangent,  0)).xyz,
        .normal    = (matrices.modelMatrix * float4(in.normal,  0)).xyz,
    };
}

struct Material
{
    float3 diffuseColor;
    float3 specularColor;
    float3 emissiveColor;
    float  shininess;

    int useDiffuseTexture;
    int useNormalMap;
    int useSpecularMap;
    int useEmissiveMap;
};

struct DirectionalLight
{
    float3 color;
    float  ambiantIntensity;
    float  diffuseIntensity;
    float  specularIntensity;
    float3 direction;
};

float3 getFragmentNormal(constant Material& material, thread const float3x3& TBN, thread texture2d<float>& normalMap, float2 uv);
float3 getFragmentDiffuseColor(constant Material& material, thread texture2d<float>& diffuseTexture, float2 uv);
float3 getFragmentSpecularColor(constant Material& material, thread texture2d<float>& specularMap, float2 uv);
float3 getFragmentEmissiveColor(constant Material& material, thread texture2d<float>& emissiveMap, float2 uv);

fragment float4 phong1(VertexOut in  [[stage_in]],
    constant Material& material      [[buffer(0)]],
    constant DirectionalLight& light [[buffer(1)]],
    texture2d<float> diffuseTexture  [[texture(1)]],
    texture2d<float> normalMap       [[texture(2)]],
    texture2d<float> specularMap     [[texture(3)]],
    texture2d<float> emissiveMap     [[texture(4)]]
)
{
    float3 fragNormal = getFragmentNormal(material, float3x3(in.tangent, in.bitangent, in.normal), normalMap, in.uv);

    float3 fragDiffuse  = getFragmentDiffuseColor (material, diffuseTexture, in.uv);
    float3 fragSpecular = getFragmentSpecularColor(material, specularMap,    in.uv);
    float3 fragEmissive = getFragmentEmissiveColor(material, emissiveMap,    in.uv);

    float3 cameraDir = normalize(float3(0, 0, -3) - in.pos);

    float diffuseFactor = dot(fragNormal, -light.direction);
    float specularFactor = dot(reflect(light.direction, fragNormal), cameraDir);

    float3 ambiant  = fragDiffuse  * light.color * light.ambiantIntensity;
    float3 diffuse  = fragDiffuse  * light.color * light.diffuseIntensity  * max(diffuseFactor, 0.0F);
    float3 specular = fragSpecular * light.color * light.specularIntensity * (specularFactor > 0.0f ? pow(specularFactor, material.shininess) : 0.0f);
    float3 emissive = fragEmissive;

    return float4(ambiant + diffuse + specular + emissive, 1.0F);
}

float3 getFragmentNormal(constant Material& material, thread const float3x3& TBN, thread texture2d<float>& normalMap, float2 uv)
{
    constexpr metal::sampler default_sampler(metal::mag_filter::nearest, metal::min_filter::nearest);

    if (material.useNormalMap != 0)
        return normalize(TBN * (normalMap.sample(default_sampler, uv).xyz * 2.0F - 1));
    else
        return normalize(TBN[2]);
}

float3 getFragmentDiffuseColor(constant Material& material, thread texture2d<float>& diffuseTexture, float2 uv)
{
    constexpr metal::sampler default_sampler(metal::mag_filter::nearest, metal::min_filter::nearest);

    if (material.useDiffuseTexture != 0)
        return diffuseTexture.sample(default_sampler, uv).xyz;
    else
        return material.diffuseColor;
}

float3 getFragmentSpecularColor(constant Material& material, thread texture2d<float>& specularMap, float2 uv)
{
    constexpr metal::sampler default_sampler(metal::mag_filter::nearest, metal::min_filter::nearest);

    if (material.useSpecularMap != 0)
        return specularMap.sample(default_sampler, uv).xyz;
    else
        return material.specularColor;
}

float3 getFragmentEmissiveColor(constant Material& material, thread texture2d<float>& emissiveMap, float2 uv)
{
    constexpr metal::sampler default_sampler(metal::mag_filter::nearest, metal::min_filter::nearest);

    if (material.useEmissiveMap != 0)
        return emissiveMap.sample(default_sampler, uv).xyz;
    else
        return material.emissiveColor;
}