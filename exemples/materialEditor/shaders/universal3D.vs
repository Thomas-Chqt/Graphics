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

out VertexOut
{
    vec3 pos;
    vec2 uv;
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
    vsOut.pos     = worldPos.xyz;
    gl_Position   = vpMatrix * worldPos;
    vsOut.uv      = uv;
    vsOut.normal  = (modelMatrix * vec4(normal, 0)).xyz;
}
