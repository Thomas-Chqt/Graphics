/*
 * ---------------------------------------------------
 * flatColorCube.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/24 19:15:28
 * ---------------------------------------------------
 */

#include <metal_stdlib>
#include "Vertex.hpp"

vertex float4 flatColorCube_vertex(uint vID [[vertex_id]],
                                   constant gfx_test::Vertex_flatColorCube *vertices    [[buffer(0)]],
                                   constant metal::float4x4                *u_MVPMatrix [[buffer(1)]])
{
    return *u_MVPMatrix * float4(vertices[vID].x, vertices[vID].y, vertices[vID].z, 1.0);
}

fragment float4 flatColorCube_fragment(constant float4* u_color [[buffer(0)]])
{
    return *u_color;
}