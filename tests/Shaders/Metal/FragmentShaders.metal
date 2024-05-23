/*
 * ---------------------------------------------------
 * FragmentShaders.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/20 17:56:59
 * ---------------------------------------------------
 */

#include "Vertex.hpp"

fragment
float4 fra1()
{
    return float4(1.0, 1.0, 1.0, 1.0);
}

fragment
float4 fra2(constant float4* u_color [[buffer(0)]])
{
    return *u_color;
}