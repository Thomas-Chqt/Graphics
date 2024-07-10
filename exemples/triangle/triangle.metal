/*
 * ---------------------------------------------------
 * triangle.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/24 18:55:58
 * ---------------------------------------------------
 */

#include <metal_stdlib>

struct VertexIN
{
    float3 position [[attribute(0)]];
};

vertex float4 triangle_vs(VertexIN vtx [[stage_in]])
{
    return float4(vtx.position.x, vtx.position.y, 0.0, 1.0);
}

fragment float4 triangle_fs()
{
    return float4(1.0, 1.0, 1.0, 1.0);
}