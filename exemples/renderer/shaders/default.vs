/*
 * ---------------------------------------------------
 * default.vs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/18 11:37:05
 * ---------------------------------------------------
 */

#version 410 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;

out VertexOut
{
    vec3 pos;
    vec2 uv;
    vec3 tangent;
    vec3 bitangent;
    vec3 normal;
} vsOut;

layout (std140) uniform matrices
{
    mat4 vpMatrix;
    mat4 modelMatrix;
};

void main()
{
    vec4 worldPos = modelMatrix * vec4(pos, 1.0);
    vec3 bitangent = cross(tangent, normal);

    vsOut.pos       = worldPos.xyz;
    gl_Position     = vpMatrix * worldPos;
    vsOut.uv        = uv;
    vsOut.tangent   = (modelMatrix * vec4(tangent,   0)).xyz;
    vsOut.bitangent = (modelMatrix * vec4(bitangent, 0)).xyz;
    vsOut.normal    = (modelMatrix * vec4(normal,    0)).xyz;
}
