/*
 * ---------------------------------------------------
 * textured.fs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/23 12:10:36
 * ---------------------------------------------------
 */

#version 410 core

in vec3 fragPos;
in vec2 fragUv;
in vec3 fragNormal;

out vec4 fragmentColor;

uniform vec3  u_cameraPos;
uniform vec3  u_light_position;
uniform vec3  u_light_color;
uniform float u_light_ambiantIntensity;
uniform float u_light_diffuseIntensity;
uniform float u_light_specularIntensity;
uniform vec3  u_material_ambiant;
uniform vec3  u_material_diffuse;
uniform vec3  u_material_specular;
uniform vec3  u_material_emissive;
uniform float u_material_shininess;

uniform sampler2D u_diffuseTexture;

void main()
{
    vec4 diffuseTextel = texture(u_diffuseTexture, fragUv);

    vec3 normal = normalize(fragNormal);

    vec3 ambiant = diffuseTextel.xyz * (u_light_color * u_light_ambiantIntensity);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    vec3 lightDirection = -normalize(u_light_position - fragPos);

    float diffuseFactor = dot(normal, -lightDirection);
    if (diffuseFactor > 0)
        diffuse = diffuseTextel.xyz * (u_light_color * u_light_diffuseIntensity) * diffuseFactor;

    float specularFactor = dot(normalize(u_cameraPos - fragPos), normalize(reflect(lightDirection, normal)));
    if (specularFactor > 0)
        specular = u_material_specular * (u_light_color * u_light_specularIntensity) * pow(specularFactor, u_material_shininess);

    fragmentColor = vec4(ambiant + diffuse + specular, diffuseTextel.w);
}
