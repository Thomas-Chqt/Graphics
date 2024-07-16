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
    vec3 normal;
    mat3 TBN;
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
};

uniform sampler2D diffuseTexture;
uniform sampler2D normalMap;

layout (std140) uniform light
{
    vec4  light_color;
    float light_ambiantIntensity;
    float light_diffuseIntensity;
    float light_specularIntensity;
    vec4  light_direction;
};

void main()
{
    vec3 normal = fsIn.normal;
    if (material_useNormalMap != 0)
        normal = fsIn.TBN * (texture(normalMap, fsIn.uv).xyz * 2.0F - 1);
    normal = normalize(normal);

    vec3 diffuseColor = material_diffuseColor.xyz;
    if (material_useDiffuseTexture != 0)
        diffuseColor = texture(diffuseTexture, fsIn.uv).xyz;

    vec3 cameraDir = normalize(vec3(0, 0, -3) - fsIn.pos);

    float diffuseFactor = dot(normal, normalize(-light_direction.xyz));
    float specularFactor = dot(reflect(light_direction.xyz, normal), cameraDir);

    vec3 ambiant  = diffuseColor               * light_color.xyz * light_ambiantIntensity;
    vec3 diffuse  = diffuseColor               * light_color.xyz * light_diffuseIntensity  * max(diffuseFactor, 0.0F);
    vec3 specular = material_specularColor.xyz * light_color.xyz * light_specularIntensity * (specularFactor > 0.0F ? pow(specularFactor, material_shininess) : 0.0F);
    vec3 emissive = material_emissiveColor.xyz;

    fragmentColor = vec4(ambiant + diffuse + specular + emissive, 1.0F);
}
