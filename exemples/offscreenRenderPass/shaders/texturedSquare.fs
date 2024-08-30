/*
 * ---------------------------------------------------
 * texturedSquare.fs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/12 14:40:09
 * ---------------------------------------------------
 */

#version 410 core

in VertexOut
{
    vec2 uv;
} fsIn;

out vec4 fragmentColor;

uniform sampler2D texture1;

void main()
{
    fragmentColor = texture(texture1, fsIn.uv);
}