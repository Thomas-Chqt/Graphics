/*
 * ---------------------------------------------------
 * default.fs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/18 11:38:19
 * ---------------------------------------------------
 */

#version 410 core

in VertexOut
{
    vec3 pos;
    vec2 uv;
    vec3 tangent;
    vec3 bitangent;
    vec3 normal;
} fsIn;

out vec4 fragmentColor;

struct DirectionalLight
{
    vec4  color;
    float ambiantIntensity;
    float diffuseIntensity;
    float specularIntensity;
    vec4  direction;
};

struct PointLight
{
    vec4  color;
    float ambiantIntensity;
    float diffuseIntensity;
    float specularIntensity;
    vec4  position;
};

layout (std140) uniform lights
{
    DirectionalLight directionalLights[32];
    uint directionalLightCount;

    PointLight pointLights[32];
    uint pointLightCount;
};

layout (std140) uniform camera
{
    vec3 camera_pos;
};

layout (std140) uniform material
{
    vec4  material_diffuseColor;
    vec4  material_specularColor;
    vec4  material_emissiveColor;
    float material_shininess;
    int   material_useDiffuseTexture;
    int   material_useNormalMap;
    int   material_useSpecularMap;
    int   material_useEmissiveMap;
};

uniform sampler2D diffuseTexture;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
uniform sampler2D emissiveMap;

vec3 getFragmentNormal();
vec3 getFragmentDiffuseColor();
vec3 getFragmentSpecularColor();
vec3 getFragmentEmissiveColor();

void main()
{
    vec3 fragNormal = getFragmentNormal();
    vec3 fragDiffuse = getFragmentDiffuseColor();
    vec3 fragSpecular = getFragmentSpecularColor();
    vec3 fragEmissive = getFragmentEmissiveColor();

    vec3 cameraDir = normalize(camera_pos - fsIn.pos);

    fragmentColor = vec4(0, 0, 0, 0);
    for (int i = 0; i < directionalLightCount; i++)
    {
        float diffuseFactor = dot(fragNormal, normalize(-directionalLights[i].direction.xyz));
        float specularFactor = dot(reflect(directionalLights[i].direction.xyz, fragNormal), cameraDir);

        vec3 ambiant  = fragDiffuse  * directionalLights[i].color.xyz * directionalLights[i].ambiantIntensity;
        vec3 diffuse  = fragDiffuse  * directionalLights[i].color.xyz * directionalLights[i].diffuseIntensity  * max(diffuseFactor, 0.0F);
        vec3 specular = fragSpecular * directionalLights[i].color.xyz * directionalLights[i].specularIntensity * (specularFactor > 0.0F ? pow(specularFactor, material_shininess) : 0.0F);
        vec3 emissive = fragEmissive;

        fragmentColor += vec4(ambiant + diffuse + specular + emissive, 1.0F);
    }
    for (int i = 0; i < pointLightCount; i++)
    {
        vec3 lightDirection = pointLights[i].position.xyz - fsIn.pos;

        float diffuseFactor = dot(fragNormal, normalize(lightDirection));
        float specularFactor = dot(reflect(lightDirection, fragNormal), cameraDir);

        vec3 ambiant  = fragDiffuse  * pointLights[i].color.xyz * pointLights[i].ambiantIntensity;
        vec3 diffuse  = fragDiffuse  * pointLights[i].color.xyz * pointLights[i].diffuseIntensity  * max(diffuseFactor, 0.0F);
        vec3 specular = fragSpecular * pointLights[i].color.xyz * pointLights[i].specularIntensity * (specularFactor > 0.0F ? pow(specularFactor, material_shininess) : 0.0F);
        vec3 emissive = fragEmissive;

        fragmentColor += vec4(ambiant + diffuse + specular + emissive, 1.0F);
    }
}

vec3 getFragmentNormal()
{
    vec3 normal;
    if (material_useNormalMap != 0)
    {
        mat3 TBN = mat3(fsIn.tangent, fsIn.bitangent, fsIn.normal);
        normal = TBN * (texture(normalMap, fsIn.uv).xyz * 2.0F - 1);
    }
    else
    {
        normal = fsIn.normal;
    }
    return normalize(normal);
}

vec3 getFragmentDiffuseColor()
{
    if (material_useDiffuseTexture != 0)
        return texture(diffuseTexture, fsIn.uv).xyz;
    else
        return material_diffuseColor.xyz;
}

vec3 getFragmentSpecularColor()
{
    if (material_useSpecularMap != 0)
        return texture(specularMap, fsIn.uv).xyz;
    else
        return material_specularColor.xyz;
}

vec3 getFragmentEmissiveColor()
{
    if (material_useEmissiveMap != 0)
        return texture(emissiveMap, fsIn.uv).xyz;
    else
        return material_emissiveColor.xyz;
}