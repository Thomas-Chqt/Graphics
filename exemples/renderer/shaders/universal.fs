/*
 * ---------------------------------------------------
 * universal.fs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/02 16:41:45
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
    vec3  ambientColor;
    uint  useAmbientTexture;

    vec3  diffuseColor;
    uint  useDiffuseTexture;

    vec3  specularColor;
    uint  useSpecularTexture;

    vec3  emissiveColor;
    uint  useEmissiveTexture;

    float shininess;
    uint  useShininessTexture;
};

in vec3 fragPos;
in vec2 fragUv;
in vec3 fragNormal;

out vec4 fragmentColor;

uniform vec3       u_cameraPos;

uniform PointLight u_pointLights[20];
uniform uint       u_pointLightsCount;

uniform Material   u_material;

uniform sampler2D  u_ambientTexture;
uniform sampler2D  u_diffuseTexture;
uniform sampler2D  u_specularTexture;
uniform sampler2D  u_emissiveTexture;
uniform sampler2D  u_shininessTexture;

vec3 computePointLight(uint lightIdx, vec3 normal, vec3 cameraDir, vec3 ambientBase, vec3 diffuseBase, vec3 specularBase, vec3 emissiveBase, float shininess)
{
    vec3 lightDir = normalize(u_pointLights[lightIdx].position - fragPos);

    float diffuseFactor  = dot(normal, lightDir);
    float specularFactor = dot(reflect(-lightDir, normal), cameraDir);

    vec3 ambiant  = ambientBase  * u_pointLights[lightIdx].color * u_pointLights[lightIdx].ambiantIntensity;
    vec3 diffuse  = diffuseBase  * u_pointLights[lightIdx].color * u_pointLights[lightIdx].diffuseIntensity  * max(diffuseFactor, 0.0f);
    vec3 specular = specularBase * u_pointLights[lightIdx].color * u_pointLights[lightIdx].specularIntensity * (specularFactor > 0.0f ? pow(specularFactor, shininess) : 0.0f);
    vec3 emissive = emissiveBase;

    return ambiant + diffuse + specular + emissive;
}

void main()
{

    vec4 ambientBase;
    if (u_material.useAmbientTexture != 0)
        ambientBase = texture(u_ambientTexture, fragUv);
    else
        ambientBase = vec4(u_material.ambientColor, 1.0f);

    vec4 diffuseBase;
    if (u_material.useDiffuseTexture != 0)
        diffuseBase = texture(u_diffuseTexture, fragUv);
    else
        diffuseBase = vec4(u_material.diffuseColor, 1.0f);

    vec4 specularBase;
    if (u_material.useSpecularTexture != 0)
        specularBase = texture(u_specularTexture, fragUv);
    else
        specularBase = vec4(u_material.specularColor, 1.0f);

    vec4 emissiveBase;
    if (u_material.useEmissiveTexture != 0)
        emissiveBase = texture(u_emissiveTexture, fragUv);
    else
        emissiveBase = vec4(u_material.emissiveColor, 1.0f);

    float shininess;
    // if (u_material.useShininessTexture != 0)
    //     shininess = texture(u_shininessTexture, fragUv);
    // else
        shininess = u_material.shininess;

    vec3 normal        = normalize(fragNormal);
    vec3 cameraDir     = normalize(u_cameraPos - fragPos);

    fragmentColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);

    for(uint i = 0; i < u_pointLightsCount; i++)
        fragmentColor += vec4(computePointLight(i, normal, cameraDir, ambientBase.xyz, diffuseBase.xyz, specularBase.xyz, emissiveBase.xyz, shininess), diffuseBase.w);
}
