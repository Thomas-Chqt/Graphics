/*
 * ---------------------------------------------------
 * texturedCube.vs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/14 14:48:24
 * ---------------------------------------------------
 */

#version 410 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;

out vec2 texCoord;

uniform mat4 u_MVPMatrix;

void main()
{
    texCoord = uv;
    gl_Position = u_MVPMatrix * vec4(pos, 1.0);
}