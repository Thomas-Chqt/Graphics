/*
 * ---------------------------------------------------
 * textured.fs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/23 12:10:36
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
uniform PointLight u_light;
uniform Material   u_material;
uniform sampler2D  u_diffuseTexture;

void main()
{
    vec4 diffuseTextel = texture(u_diffuseTexture, fragUv);

    vec3 normal = normalize(fragNormal);

    vec3 ambiant = diffuseTextel.xyz * (u_light.color * u_light.ambiantIntensity);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    vec3 lightDirection = -normalize(u_light.position - fragPos);

    float diffuseFactor = dot(normal, -lightDirection);
    if (diffuseFactor > 0)
        diffuse = diffuseTextel.xyz * (u_light.color * u_light.diffuseIntensity) * diffuseFactor;

    float specularFactor = dot(normalize(u_cameraPos - fragPos), normalize(reflect(lightDirection, normal)));
    if (specularFactor > 0)
        specular = u_material.specular * (u_light.color * u_light.specularIntensity) * pow(specularFactor, u_material.shininess);

    fragmentColor = vec4(ambiant + diffuse + specular, diffuseTextel.w);
}
