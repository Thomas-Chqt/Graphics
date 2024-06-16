/*
 * ---------------------------------------------------
 * diffuseLight.vs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/15 21:20:31
 * ---------------------------------------------------
 */

#version 410 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 norm;

out vec2 texCoord;
out vec3 normal;

uniform mat4 u_modelMatrix;
uniform mat4 u_vpMatrix;

void main()
{
    normal      = u_modelMatrix * norm;
    texCoord    = uv;
    gl_Position = u_vpMatrix * u_modelMatrix * vec4(pos, 1.0);
}