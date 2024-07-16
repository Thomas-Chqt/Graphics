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
        .tangent   = (modelMatrix * float4(in.tangent, 0)).xyz,
        .bitangent = (modelMatrix * float4(bitangent,  0)).xyz,
        .normal    = (modelMatrix * float4(in.normal,  0)).xyz,
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
    texture2d<float> diffuseTexture  [[texture(1)]],
    texture2d<float> normalMap       [[texture(2)]]
)
{
    constexpr metal::sampler default_sampler(metal::mag_filter::nearest, metal::min_filter::nearest);

    float3x3 TBN = float3x3(in.tangent, in.bitangent, in.normal);

    float3 normal = in.normal;
    if (material.useNormalMap)
        normal = TBN * (normalMap.sample(default_sampler, in.uv).xyz * 2.0F - 1);
    normal = normalize(normal);

    float3 diffuseColor = material.diffuseColor;
    if (material.useDiffuseTexture)
        diffuseColor = diffuseTexture.sample(default_sampler, in.uv).xyz;

    float3 cameraDir = normalize(float3(0, 0, -3) - in.pos);

    float diffuseFactor = dot(normal, normalize(-light.direction));
    float specularFactor = dot(reflect(light.direction, normal), cameraDir);

    float3 ambiant  = diffuseColor           * light.color * light.ambiantIntensity;
    float3 diffuse  = diffuseColor           * light.color * light.diffuseIntensity  * max(diffuseFactor, 0.0F);
    float3 specular = material.specularColor * light.color * light.specularIntensity * (specularFactor > 0.0f ? pow(specularFactor, material.shininess) : 0.0f);
    float3 emissive = material.emissiveColor;

    return float4(ambiant + diffuse + specular + emissive, 1.0F);
}
