/*
 * ---------------------------------------------------
 * universal.fs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/24 13:27:12
 * ---------------------------------------------------
 */

#version 410 core

struct Material
{
    vec3  ambiant;
    vec3  diffuse;
    vec3  specular;
    vec3  emissive;
    float shininess;
    uint  useDiffuseMap;
};

struct PointLight
{
    vec3  position;
    vec3  color;
    float ambiantIntensity;
    float diffuseIntensity;
    float specularIntensity;
};

in vec3 fragPos;
in vec2 fragUv;
in vec3 fragNormal;

out vec4 fragmentColor;

uniform vec3       u_cameraPos;
uniform PointLight u_pointLights[20];
uniform Material   u_material;
uniform sampler2D  u_diffuseTexture;

vec3 computePointLight(int lightIdx, vec3 normal, vec3 cameraDir, vec3 diffuseTexel)
{
    vec3 lightDir = normalize(u_pointLights[lightIdx].position - fragPos);

    float diffuseFactor  = dot(normal, lightDir);
    float specularFactor = dot(reflect(-lightDir, normal), cameraDir);

    vec3 ambiant  = u_material.useDiffuseMap == 1 ? diffuseTexel : u_material.ambiant;
    vec3 diffuse  = u_material.useDiffuseMap == 1 ? diffuseTexel : u_material.diffuse;
    vec3 specular = u_material.specular;
    vec3 emissive = u_material.emissive;

    ambiant  *= u_pointLights[lightIdx].color * u_pointLights[lightIdx].ambiantIntensity;
    diffuse  *= u_pointLights[lightIdx].color * u_pointLights[lightIdx].diffuseIntensity * max(diffuseFactor, 0.0f);
    specular *= u_pointLights[lightIdx].color * u_pointLights[lightIdx].specularIntensity * (specularFactor > 0.0f ? pow(specularFactor, u_material.shininess) : 0.0f);

    return ambiant + diffuse + specular + emissive;
}

void main()
{
    vec3 normal = normalize(fragNormal);
    vec3 cameraDir = normalize(u_cameraPos - fragPos);

    fragmentColor = vec4(computePointLight(0, normal, cameraDir, u_material.useDiffuseMap == 1 ? texture(u_diffuseTexture, fragUv).xyz : vec3(0, 0, 0)), 1.0);
}
