/*
 * ---------------------------------------------------
 * ambiantLight.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/15 16:25:58
 * ---------------------------------------------------
 */

#include <metal_stdlib>
#include "Vertex.hpp"

using namespace metal;

struct VertexOut
{
    float4 pos [[position]];
    float2 uv;
};

vertex VertexOut ambiantLight_vs(
    uint vID                         [[vertex_id]],
    constant Vertex *vertices        [[buffer(0)]],
    constant float4x4& u_modelMatrix [[buffer(1)]],
    constant float4x4& u_vpMatrix    [[buffer(2)]]
)
{
    return (VertexOut){
        .pos =  u_vpMatrix * u_modelMatrix * float4(vertices[vID].pos, 1.0),
        .uv  = vertices[vID].uv
    };
}

fragment float4 ambiantLight_fs(
    VertexOut in                       [[stage_in]],
    texture2d<float> u_texture         [[texture(0)]],
    constant float4& u_ambiantColor    [[buffer(0)]],
    constant float& u_ambiantIntensity [[buffer(1)]]
)
{
    constexpr metal::sampler textureSampler(metal::mag_filter::nearest, metal::min_filter::nearest);
    float4 pixelColor = u_texture.sample(textureSampler, in.uv);
    return float4(pixelColor.xyz * u_ambiantColor.xyz * u_ambiantIntensity, pixelColor.w);
}