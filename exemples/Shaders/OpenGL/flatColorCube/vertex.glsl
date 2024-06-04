/*
 * ---------------------------------------------------
 * vertex.glsl
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/24 19:35:44
 * ---------------------------------------------------
 */

#version 410 core

layout (location = 0) in vec3 pos;

uniform mat4 u_MVPMatrix;

void main()
{
    gl_Position = u_MVPMatrix * vec4(pos, 1.0);
}
