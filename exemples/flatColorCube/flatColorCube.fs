/*
 * ---------------------------------------------------
 * fragmentUniform.fs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/07 16:25:50
 * ---------------------------------------------------
 */

#version 410 core

uniform vec4 u_color;

out vec4 fragmentColor;
        
void main()
{
    fragmentColor = u_color;
}