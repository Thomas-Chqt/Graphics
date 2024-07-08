/*
 * ---------------------------------------------------
 * metalShaders.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/07 09:27:04
 * ---------------------------------------------------
 */

#include <metal_stdlib>
#include "shaderShared.hpp"

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
    constant float4x4& u_vpMatrix    [[buffer(1)]],
    constant float4x4& u_modelMatrix [[buffer(2)]]
)
{
    float4 worldPos = u_modelMatrix * float4(vertices[vID].pos, 1.0);
    return (VertexOut){
        .pos     = worldPos.xyz,
        .clipPos = u_vpMatrix * worldPos,
        .uv      = vertices[vID].uv,
        .normal  = (u_modelMatrix * float4(vertices[vID].normal, 0)).xyz
    };
}

fragment float4 phong1(VertexOut in [[stage_in]],
    constant DirectionalLight& u_light [[buffer(0)]],
    constant Material& u_material      [[buffer(1)]]
)
{
    return u_material.baseColor;
}