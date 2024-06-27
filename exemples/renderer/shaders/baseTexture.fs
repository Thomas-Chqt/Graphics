/*
 * ---------------------------------------------------
 * baseTexture.fs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/24 13:27:12
 * ---------------------------------------------------
 */

#version 410 core

struct PointLight
{
    vec3  position;
    vec3  color;
    float ambiantIntensity;
    float diffuseIntensity;
    float specularIntensity;
};

struct Material
{
    vec3  specularColor;
    vec3  emissiveColor;
    float shininess;
};

in vec3 fragPos;
in vec2 fragUv;
in vec3 fragNormal;

out vec4 fragmentColor;

uniform vec3       u_cameraPos;
uniform PointLight u_pointLights[20];
uniform uint       u_pointLightsCount;
uniform Material   u_material;
uniform sampler2D  u_diffuseTexture;

vec3 computePointLight(uint lightIdx, vec3 normal, vec3 cameraDir, vec3 diffuseTexel)
{
    vec3 lightDir = normalize(u_pointLights[lightIdx].position - fragPos);

    float diffuseFactor  = dot(normal, lightDir);
    float specularFactor = dot(reflect(-lightDir, normal), cameraDir);

    vec3 ambiant  = diffuseTexel             * u_pointLights[lightIdx].color * u_pointLights[lightIdx].ambiantIntensity;
    vec3 diffuse  = diffuseTexel             * u_pointLights[lightIdx].color * u_pointLights[lightIdx].diffuseIntensity  * max(diffuseFactor, 0.0f);
    vec3 specular = u_material.specularColor * u_pointLights[lightIdx].color * u_pointLights[lightIdx].specularIntensity * (specularFactor > 0.0f ? pow(specularFactor, u_material.shininess) : 0.0f);
    vec3 emissive = u_material.emissiveColor;

    return ambiant + diffuse + specular + emissive;
}

void main()
{
    vec4 diffuseTexel = texture(u_diffuseTexture, fragUv);
    vec3 normal        = normalize(fragNormal);
    vec3 cameraDir     = normalize(u_cameraPos - fragPos);

    fragmentColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    for(uint i = 0; i < u_pointLightsCount; i++)
        fragmentColor += vec4(computePointLight(i, normal, cameraDir, diffuseTexel.xyz), 1.0);
}
