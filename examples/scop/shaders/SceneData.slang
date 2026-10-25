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

#include "shader.h"

#ifdef __cplusplus

#include "Light.slang"

namespace shader
{
#else

module SceneData;

import Light;

#endif

#if defined(__cplusplus) && defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4324)
#endif
SLANG_PUBLIC struct SceneData
{
    SLANG_PUBLIC float3 cameraPosition;
    FLOAT3_PADDING(0);

    SLANG_PUBLIC float3 ambientLightColor;
    FLOAT3_PADDING(1);

    SLANG_PUBLIC float exposure;

    SLANG_PUBLIC int directionalLightCount;
    SLANG_PUBLIC DirectionalLight directionalLights[8]; // NOLINT(cppcoreguidelines-avoid-c-arrays)

    SLANG_PUBLIC int pointLightCount = 0;
    SLANG_PUBLIC PointLight pointLights[8]; // NOLINT(cppcoreguidelines-avoid-c-arrays)
};
#if defined(__cplusplus) && defined(_MSC_VER)
#pragma warning(pop)
#endif

#ifdef __cplusplus
}
#endif

#endif
