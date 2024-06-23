/*
 * ---------------------------------------------------
 * flatColor.fs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/23 12:03:11
 * ---------------------------------------------------
 */

#version 410 core

struct PointLight
{
    vec3 position;
    vec3 color;
    float ambiantIntensity;
    float diffuseIntensity;
    float specularIntensity;
};

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

uniform vec3       u_cameraPos;
uniform PointLight u_light[20];
uniform Material   u_material;

void main()
{
    vec3 normal = normalize(fragNormal);

    vec3 ambiant = u_material.ambiant * (u_light[0].color * u_light[0].ambiantIntensity);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    vec3 lightDirection = -normalize(u_light[0].position - fragPos);

    float diffuseFactor = dot(normal, -lightDirection);
    if (diffuseFactor > 0)
        diffuse = u_material.diffuse * (u_light[0].color * u_light[0].diffuseIntensity) * diffuseFactor;

    float specularFactor = dot(normalize(u_cameraPos - fragPos), normalize(reflect(lightDirection, normal)));
    if (specularFactor > 0)
        specular = u_material.specular * (u_light[0].color * u_light[0].specularIntensity) * pow(specularFactor, u_material.shininess);

    fragmentColor = vec4(ambiant + diffuse + specular, 1.0);
}
