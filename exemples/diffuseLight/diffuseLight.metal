/*
 * ---------------------------------------------------
 * diffuseLight.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/15 21:20:28
 * ---------------------------------------------------
 */

#include <metal_stdlib>
#include "Vertex.hpp"

using namespace metal;

struct VertexOut
{
    float4 pos [[position]];
    float2 uv;
    float3 normal;
};

vertex VertexOut diffuseLight_vs(
    uint vID                         [[vertex_id]],
    constant Vertex *vertices        [[buffer(0)]],
    constant float4x4& u_modelMatrix [[buffer(1)]],
    constant float4x4& u_vpMatrix    [[buffer(2)]]
)
{
    return (VertexOut){
        .pos    = u_vpMatrix * u_modelMatrix * float4(vertices[vID].pos, 1.0),
        .uv     = vertices[vID].uv,
        .normal = (u_modelMatrix * float4(vertices[vID].normal, 0)).xyz
    };
}

fragment float4 diffuseLight_fs(
    VertexOut in                        [[stage_in]],
    texture2d<float> u_texture          [[texture(0)]],
    constant float4& u_diffuseColor     [[buffer(0)]],
    constant float& u_diffuseIntensity  [[buffer(1)]],
    constant float3& u_diffuseDirection [[buffer(2)]]
)
{
    constexpr metal::sampler textureSampler(metal::mag_filter::nearest, metal::min_filter::nearest);
    float4 pixelColor = u_texture.sample(textureSampler, in.uv);
    float diffuseFactor = dot(normalize(in.normal), -u_diffuseDirection);
    return float4(pixelColor.xyz * u_diffuseColor.xyz * diffuseFactor * u_diffuseIntensity, pixelColor.w);
}