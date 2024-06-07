/*
 * ---------------------------------------------------
 * indexedShape.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/07 15:55:07
 * ---------------------------------------------------
 */

#include <metal_stdlib>
#include "Vertex.hpp"

vertex float4 indexedShape_vs(uint vID [[vertex_id]],
                              constant Vertex *vertices [[buffer(0)]])
{
    return float4(vertices[vID].x, vertices[vID].y, 0.0, 1.0);
}

fragment float4 indexedShape_fs()
{
    return float4(1.0, 1.0, 1.0, 1.0);
}