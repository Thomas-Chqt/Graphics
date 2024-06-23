/*
 * ---------------------------------------------------
 * lightCube.fs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/23 12:09:10
 * ---------------------------------------------------
 */

#version 410 core

struct Material
{
    vec3 ambiant;
    vec3 diffuse;
    vec3 specular;
    vec3 emissive;
    float shininess;
};

in vec3 fragPos;
in vec2 fragUv;
in vec3 fragNormal;

out vec4 fragmentColor;

uniform Material   u_material;

void main()
{
    fragmentColor = vec4(u_material.emissive, 1.0);
}
