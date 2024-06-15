/*
 * ---------------------------------------------------
 * ambiantLight.vs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/15 16:26:04
 * ---------------------------------------------------
 */

#version 410 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;

out vec2 texCoord;

uniform mat4 u_modelMatrix;
uniform mat4 u_vpMatrix;

void main()
{
    texCoord = uv;
    gl_Position = u_vpMatrix * u_modelMatrix * vec4(pos, 1.0);
}