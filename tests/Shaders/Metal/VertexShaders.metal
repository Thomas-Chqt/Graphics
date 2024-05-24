/*
 * ---------------------------------------------------
 * VertexShaders.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/20 17:57:21
 * ---------------------------------------------------
 */

#include <metal_stdlib>

#include "Vertex.hpp"

vertex
float4 vtx1(uint vID [[vertex_id]],
            constant gfx_test::Vertex *vertices [[buffer(0)]])
{
    return float4(vertices[vID].x, vertices[vID].y, 0.0, 1.0);
}

vertex
float4 vtx2(uint vID [[vertex_id]],
            constant gfx_test::Vertex3D *vertices    [[buffer(0)]],
            constant metal::float4x4    *u_MVPMatrix [[buffer(1)]])
{
    return *u_MVPMatrix * float4(vertices[vID].x, vertices[vID].y, vertices[vID].z, 1.0);
}