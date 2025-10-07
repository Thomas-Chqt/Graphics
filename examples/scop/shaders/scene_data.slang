/*
 * ---------------------------------------------------
 * scene_data.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/10/05 17:05:24
 * ---------------------------------------------------
 */

module scene_data;

import light;

public struct SceneData
{
    public float3 cameraPosition;
#ifdef __SPIRV__
    float _padding0;
#endif
    public float3 ambientLightColor;

#ifdef __SPIRV__
    float _padding1;
#endif

    public int directionalLightCount;
    public DirectionalLight directionalLights[8];

    public int pointLightCount = 0;
    public PointLight pointLights[8];

};
