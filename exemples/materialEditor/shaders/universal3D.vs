/*
 * ---------------------------------------------------
 * universal3D.vs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/13 11:38:32
 * ---------------------------------------------------
 */

#version 410 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out VertexOut
{
    vec3 pos;
    vec2 uv;
    vec3 normal;
    mat3 TBN;
} vsOut;

layout (std140) uniform matrices
{
    mat4 vpMatrix;
    mat4 modelMatrix;
};

void main()
{
    vec3 T = (modelMatrix * vec4(tangent,   0)).xyz;
    vec3 B = (modelMatrix * vec4(bitangent, 0)).xyz;
    vec3 N = (modelMatrix * vec4(normal,    0)).xyz;


    vec4 worldPos = modelMatrix * vec4(pos, 1.0);
    vsOut.pos     = worldPos.xyz;
    gl_Position   = vpMatrix * worldPos;
    vsOut.uv      = uv;
    vsOut.normal  = N;
    vsOut.TBN     = mat3(T, B, N);
}
