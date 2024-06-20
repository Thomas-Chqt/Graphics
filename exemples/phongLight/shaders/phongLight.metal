/*
 * ---------------------------------------------------
 * phongLight.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/19 18:22:10
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

vertex VertexOut phongLight_vs(
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

fragment float4 phongLight_fs(
    VertexOut in                        [[stage_in]],
    constant float4& u_diffuseColor     [[buffer(0)]],
    constant float& u_diffuseIntensity  [[buffer(1)]],
    constant float3& u_diffuseDirection [[buffer(2)]]
)
{
    float4 baseColor = float4(1, 1, 1, 1);
    float diffuseFactor = dot(normalize(in.normal), -u_diffuseDirection);
    return float4(baseColor.xyz * u_diffuseColor.xyz * diffuseFactor * u_diffuseIntensity, baseColor.w);
}
