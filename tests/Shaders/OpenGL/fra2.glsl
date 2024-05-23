/*
 * ---------------------------------------------------
 * fra2.glsl
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/23 23:16:56
 * ---------------------------------------------------
 */

#version 410 core

uniform vec4 u_color;

out vec4 fragmentColor;
        
void main()
{
    fragmentColor = u_color;
}
