/*
 * ---------------------------------------------------
 * SceneData.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/11/01 17:03:28
 * ---------------------------------------------------
 */

#ifndef SCENEDATA_SLANG
#define SCENEDATA_SLANG

#include "shared_types.h"

#ifdef __cplusplus

#include "Light.slang"

namespace shader
{
#else

module SceneData;

import Light;

#endif

SLAND_PUBLIC struct SceneData
{
    SLAND_PUBLIC float3 cameraPosition;
    FLOAT3_PADDING(0);

    SLAND_PUBLIC float3 ambientLightColor;
    FLOAT3_PADDING(1);

    SLAND_PUBLIC int directionalLightCount;
    SLAND_PUBLIC DirectionalLight directionalLights[8]; // NOLINT(cppcoreguidelines-avoid-c-arrays)

    SLAND_PUBLIC int pointLightCount = 0;
    SLAND_PUBLIC PointLight pointLights[8]; // NOLINT(cppcoreguidelines-avoid-c-arrays)
};

#ifdef __cplusplus
}
#endif

#endif
