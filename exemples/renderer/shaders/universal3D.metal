/*
 * ---------------------------------------------------
 * universal3D.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/21 17:57:43
 * ---------------------------------------------------
 */

#include <metal_stdlib>
#include "Vertex.hpp"

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