/*
 * ---------------------------------------------------
 * fragment.glsl
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/30 11:06:50
 * ---------------------------------------------------
 */

#version 410 core

uniform vec4 u_color;

out vec4 fragmentColor;
        
void main()
{
    fragmentColor = u_color;
}