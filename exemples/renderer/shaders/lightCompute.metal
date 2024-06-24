/*
 * ---------------------------------------------------
 * lightCompute.metal
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/24 13:01:45
 * ---------------------------------------------------
 */
#include "lightCompute.metal.h"

#include <metal_stdlib>

using namespace metal;

float3 computePointLight(constant PointLight& light, const thread FragmentInfos& frag, float3 cameraDir)
{
    float3 lightDir = normalize(light.position - frag.position);

    float diffuseFactor  = dot(frag.normal, lightDir);
    float specularFactor = dot(reflect(-lightDir, frag.normal), cameraDir);

    float3 ambiant  = frag.ambiant  * light.color * light.ambiantIntensity;
    float3 diffuse  = frag.diffuse  * light.color * light.diffuseIntensity * max(diffuseFactor, 0.0f);
    float3 specular = frag.specular * light.color * light.specularIntensity * (specularFactor > 0.0f ? pow(specularFactor, frag.shininess) : 0.0f);
    float3 emissive = frag.emissive;

    return ambiant + diffuse + specular + emissive;
}