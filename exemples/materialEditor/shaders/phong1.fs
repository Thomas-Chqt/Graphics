/*
 * ---------------------------------------------------
 * phong1.fs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/13 11:42:39
 * ---------------------------------------------------
 */

#version 410 core

in VertexOut
{
    vec3 pos;
    vec2 uv;
    vec3 normal;
} fsIn;

out vec4 fragmentColor;

void main()
{
    fragmentColor = vec4(1.0, 1.0, 1.0, 1.0);
}
