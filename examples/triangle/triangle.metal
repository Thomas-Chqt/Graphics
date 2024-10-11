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

struct Colors
{
    float3 red;
    float3 green;
    float3 blue;
};

fragment float4 triangle_fs(constant Colors& colors [[buffer(0)]])
{
    return float4(colors.blue, 1.0);
}