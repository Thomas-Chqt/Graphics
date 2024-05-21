/*
 * ---------------------------------------------------
 * VertexShaders.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/20 17:57:21
 * ---------------------------------------------------
 */

#include "Vertex.hpp"

vertex
float4 vtx1(uint vertexID [[vertex_id]], constant gfx_test::Vertex *vertices [[buffer(0)]])
{
    return float4(vertices[vertexID].x, vertices[vertexID].y, 0.0, 1.0);
}