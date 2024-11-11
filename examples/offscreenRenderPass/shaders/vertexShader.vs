/*
 * ---------------------------------------------------
 * vertexShader.vs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/12 14:40:18
 * ---------------------------------------------------
 */

#version 410 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 uv;

out VertexOut
{
    vec2 uv;
} vsOut;

void main()
{
    gl_Position = vec4(pos, 0.0, 1.0);
    vsOut.uv    = uv;
}
