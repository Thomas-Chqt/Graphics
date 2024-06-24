/*
 * ---------------------------------------------------
 * lightCompute.metal.h
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/24 13:02:15
 * ---------------------------------------------------
 */

#ifndef LIGHTCOMPUTE_METAL_H
# define LIGHTCOMPUTE_METAL_H

#include "ShaderDatas.hpp"

struct FragmentInfos
{
    float3 position;
    float3 normal;
    float3 ambiant;
    float3 diffuse;
    float3 specular;
    float3 emissive;
    float  shininess;
};

float3 computePointLight(constant PointLight&, const thread FragmentInfos&, float3 cameraDir);

#endif // LIGHTCOMPUTE_METAL_H