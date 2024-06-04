/*
 * ---------------------------------------------------
 * vertex.glsl
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/30 11:06:34
 * ---------------------------------------------------
 */

#version 410 core

layout (location = 0) in vec2 pos;

uniform mat3 u_MVPMatrix;

void main()
{
    gl_Position = vec4(u_MVPMatrix * vec3(pos, 1.0), 1.0);
}

