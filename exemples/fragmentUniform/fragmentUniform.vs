/*
 * ---------------------------------------------------
 * fragmentUniform.vs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/07 16:25:56
 * ---------------------------------------------------
 */

#version 410 core

layout (location = 0) in vec2 pos;

void main()
{
    gl_Position = vec4(pos, 0.0, 1.0);
}
