/*
 * ---------------------------------------------------
 * phong1.fs
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/13 11:42:39
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

layout (std140) uniform light
{
    vec4  light_color;
    float light_ambiantIntensity;
    float light_diffuseIntensity;
    float light_specularIntensity;
    vec4  light_direction;
};


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

    vec3 cameraDir = normalize(vec3(0, 0, -3) - fsIn.pos);

    float diffuseFactor = dot(fragNormal, normalize(-light_direction.xyz));
    float specularFactor = dot(reflect(light_direction.xyz, fragNormal), cameraDir);

    vec3 ambiant  = fragDiffuse  * light_color.xyz * light_ambiantIntensity;
    vec3 diffuse  = fragDiffuse  * light_color.xyz * light_diffuseIntensity  * max(diffuseFactor, 0.0F);
    vec3 specular = fragSpecular * light_color.xyz * light_specularIntensity * (specularFactor > 0.0F ? pow(specularFactor, material_shininess) : 0.0F);
    vec3 emissive = fragEmissive;

    fragmentColor = vec4(ambiant + diffuse + specular + emissive, 1.0F);
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