/*
 * ---------------------------------------------------
 * universal.vs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/24 13:26:35
 * ---------------------------------------------------
 */

#version 410 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

out vec3 fragPos;
out vec2 fragUv;
out vec3 fragNormal;

uniform mat4 u_modelMatrix;
uniform mat4 u_vpMatrix;

void main()
{
    fragPos     = (u_modelMatrix * vec4(pos, 1.0)).xyz;
    fragUv      = uv;
    fragNormal  = (u_modelMatrix * vec4(normal, 0.0)).xyz;
    gl_Position = u_vpMatrix * u_modelMatrix * vec4(pos, 1.0);
}
