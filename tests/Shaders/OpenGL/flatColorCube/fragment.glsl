/*
 * ---------------------------------------------------
 * fragment.glsl
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/24 19:36:04
 * ---------------------------------------------------
 */

#version 410 core

uniform vec4 u_color;

out vec4 fragmentColor;
        
void main()
{
    fragmentColor = u_color;
}