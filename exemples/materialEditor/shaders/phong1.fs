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
} fsIn;

out vec4 fragmentColor;

layout (std140) uniform material
{
    vec4 baseColor;
};

layout (std140) uniform light
{
    vec3  lightColor;
    float _pad1;
    float lightAmbiantIntensity;
    float lightDiffuseIntensity;
    float lightSpecularIntensity;
    float _pad2;
    vec3  lightDirection;
};

void main()
{
    fragmentColor = baseColor;

    float diffuseFactor = dot(normalize(fsIn.normal), normalize(-lightDirection));

    vec3 ambiant = baseColor.xyz * lightColor * lightAmbiantIntensity;
    vec3 diffuse = baseColor.xyz * lightColor * lightDiffuseIntensity * max(diffuseFactor, 0.0F);

    fragmentColor = vec4(ambiant + diffuse, baseColor.w);
}
