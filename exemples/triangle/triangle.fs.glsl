/*
 * ---------------------------------------------------
 * fragment.glsl
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/24 19:36:04
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
