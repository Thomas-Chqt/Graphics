/*
 * ---------------------------------------------------
 * lightCube.fs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/23 12:09:10
 * ---------------------------------------------------
 */

#version 410 core

in vec3 fragPos;
in vec2 fragUv;
in vec3 fragNormal;

out vec4 fragmentColor;

uniform vec3  u_material_ambiant;
uniform vec3  u_material_diffuse;
uniform vec3  u_material_specular;
uniform vec3  u_material_emissive;
uniform float u_material_shininess;

void main()
{
    fragmentColor = vec4(u_material_emissive, 1.0);
}
