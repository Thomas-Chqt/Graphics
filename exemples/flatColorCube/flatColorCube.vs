/*
 * ---------------------------------------------------
 * fragmentUniform.vs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/07 16:25:56
 * ---------------------------------------------------
 */

#version 410 core

layout (location = 0) in vec3 pos;

uniform mat4 u_MVPMatrix;

void main()
{
    gl_Position = u_MVPMatrix * vec4(pos, 1.0);
}