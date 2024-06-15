/*
 * ---------------------------------------------------
 * texturedCube.fs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/14 14:49:20
 * ---------------------------------------------------
 */

#version 410 core

uniform sampler2D u_texture;

in vec2 texCoord;

out vec4 fragmentColor;
        
void main()
{
    fragmentColor = texture(u_texture, texCoord);
}
