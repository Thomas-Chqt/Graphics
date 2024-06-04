/*
 * ---------------------------------------------------
 * indexedShape.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/24 18:55:58
 * ---------------------------------------------------
 */

#include <metal_stdlib>
#include "Vertex.hpp"

vertex float4 indexedShape_vertex(uint vID [[vertex_id]],
                                  constant gfx_test::Vertex_indexedShape *vertices [[buffer(0)]])
{
    return float4(vertices[vID].x, vertices[vID].y, 0.0, 1.0);
}

fragment float4 indexedShape_fragment()
{
    return float4(1.0, 1.0, 1.0, 1.0);
}