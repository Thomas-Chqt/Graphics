/*
 * ---------------------------------------------------
 * texturedCube.fs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/14 14:49:20
 * ---------------------------------------------------
 */

#version 410 core

in vec2 texCoord;

out vec4 fragmentColor;

uniform sampler2D u_texture;

void main()
{
    fragmentColor = texture(u_texture, texCoord);
}
