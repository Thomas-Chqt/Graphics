/*
 * ---------------------------------------------------
 * vertex.glsl
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/24 19:35:44
 * ---------------------------------------------------
 */

#version 410 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 uv;

out vec2 texCoord;

void main()
{
    gl_Position = vec4(pos, 0.0, 1.0);
}
