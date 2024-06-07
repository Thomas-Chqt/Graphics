/*
 * ---------------------------------------------------
 * triangle.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/24 18:55:58
 * ---------------------------------------------------
 */

#include <metal_stdlib>
#include "Vertex.hpp"

vertex float4 triangle_vs(uint vID [[vertex_id]],
                          constant Vertex* vertices [[buffer(0)]])
{
    return float4(vertices[vID].x, vertices[vID].y, 0.0, 1.0);
}

fragment float4 triangle_fs()
{
    return float4(1.0, 1.0, 1.0, 1.0);
}