/*
 * ---------------------------------------------------
 * Light.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/11/01 17:01:32
 * ---------------------------------------------------
 */

#ifndef LIGHT_SLANG
#define LIGHT_SLANG

#include "shared_types.h"

#ifdef __cplusplus
namespace shader
{
#else

module Light;

#endif

SLAND_PUBLIC struct CPP_ALIGNAS(16) DirectionalLight
{
    SLAND_PUBLIC float3 position;
    FLOAT3_PADDING(0);
    SLAND_PUBLIC float3 color;
    FLOAT3_PADDING(1);
};

SLAND_PUBLIC struct CPP_ALIGNAS(16) PointLight
{
    SLAND_PUBLIC float3 position;
    FLOAT3_PADDING(0);
    SLAND_PUBLIC float3 color;
    FLOAT3_PADDING(1);
    SLAND_PUBLIC float attenuation;
};

#ifdef __cplusplus
}
#endif

#endif
