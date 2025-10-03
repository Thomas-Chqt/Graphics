/*
 * ---------------------------------------------------
 * scop_shader.h
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/10/03 07:24:55
 * ---------------------------------------------------
 */

#ifndef SCOP_SHADER_H
#define SCOP_SHADER_H

#ifdef __cplusplus
#include <glm/glm.hpp>
#endif

#ifndef __cplusplus
# define ParameterBlockSingleCB(T, idx, name) \
     struct T##PB##idx                        \
     {                                        \
         ConstantBuffer<T> data;              \
     };                                       \
     ParameterBlock<T##PB##idx> name;
# ifdef __SPIRV__
#  define NEED_FLOAT3_PADDING
# endif
#else
# define float3 glm::vec3
# define NEED_FLOAT3_PADDING
#endif

#ifdef NEED_FLOAT3_PADDING
# define float3Pad(name, idx) float3 name = float3(0); float _padding##idx = 0
#else
# define float3Pad(name, idx) float3 name = float3(0)
#endif

#ifdef __cplusplus
namespace shader {
#endif

#ifdef __cplusplus
struct alignas(16) DirectionalLight
#else
struct DirectionalLight
#endif
{
    float3Pad(position, 0);
    float3Pad(color, 1);
};

#ifdef __cplusplus
struct alignas(16) PointLight
#else
struct PointLight
#endif
{
    float3Pad(position, 0);
    float3Pad(color, 1);
    float attenuation = 0;
};

#ifdef __cplusplus
struct alignas(16) SceneData
#else
struct SceneData
#endif
{
    float3Pad(cameraPosition, 0);
    float3Pad(ambientLightColor, 1);

    int directionalLightCount = 0;
    DirectionalLight directionalLights[8]; // NOLINT(cppcoreguidelines-avoid-c-arrays)

    int pointLightCount = 0;
    PointLight pointLights[8]; // NOLINT(cppcoreguidelines-avoid-c-arrays)
};

#ifdef __cplusplus
struct alignas(16) FlatColorMaterial
#else
struct FlatColorMaterial
#endif
{
    float3Pad(diffuseColor, 0);
    float shininess;
    float specular;
};

#ifdef __cplusplus
}
#endif

#endif // SCOP_SHADER_H
