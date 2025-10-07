/*
 * ---------------------------------------------------
 * light.slang
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/10/05 14:59:11
 * ---------------------------------------------------
 */

module light;

public struct DirectionalLight
{
    public float3 position;
#ifdef __SPIRV__
    float _padding0;
#endif
    public float3 color;
#ifdef __SPIRV__
    float _padding1;
#endif
};

public struct PointLight
{
    public float3 position;
#ifdef __SPIRV__
    float _padding0;
#endif
    public float3 color;
#ifdef __SPIRV__
    float _padding1;
#endif
    public float attenuation;
};
