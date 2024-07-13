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
    float4 baseColor;
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
    constant DirectionalLight& light [[buffer(1)]]
)
{
    float diffuseFactor = dot(normalize(in.normal), normalize(-light.direction));

    float3 ambiant = material.baseColor.xyz * light.color * light.ambiantIntensity;
    float3 diffuse = material.baseColor.xyz * light.color * light.diffuseIntensity * max(diffuseFactor, 0.0F);

    return float4(ambiant + diffuse, material.baseColor.w);
}