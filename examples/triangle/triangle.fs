/*
 * ---------------------------------------------------
 * fragment.glsl
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/24 19:36:04
 * ---------------------------------------------------
 */

#version 410 core

out vec4 fragmentColor;
        
layout (std140) uniform colors
{
    vec3 red;
    vec3 green;
    vec3 blue;
};

void main()
{
    fragmentColor = vec4(blue, 1.0);
}
