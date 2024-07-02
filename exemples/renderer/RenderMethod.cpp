/*
 * ---------------------------------------------------
 * RenderMethod.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/25 12:08:14
 * ---------------------------------------------------
 */

#include "RenderMethod.hpp"
#include "Entity.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "ShaderStructs.hpp"

template<>
void addToDescriptor<VertexShader::universal3D>(gfx::GraphicPipeline::Descriptor& descriptor)
{
    #ifdef GFX_METAL_ENABLED
        descriptor.metalVSFunction = "universal3D";
    #endif 
    #if GFX_OPENGL_ENABLED
        descriptor.openglVSCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/universal3D.vs");
    #endif
}

template<>
void addToDescriptor<FragmentShader::universal>(gfx::GraphicPipeline::Descriptor& descriptor)
{
    #ifdef GFX_METAL_ENABLED
        descriptor.metalFSFunction = "universal_fs";
    #endif 
    #if GFX_OPENGL_ENABLED
        descriptor.openglFSCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/universal.fs");
    #endif
}

template<>
void addToDescriptor<FragmentShader::baseColor>(gfx::GraphicPipeline::Descriptor& descriptor)
{
    #ifdef GFX_METAL_ENABLED
        descriptor.metalFSFunction = "baseColor_fs";
    #endif 
    #if GFX_OPENGL_ENABLED
        descriptor.openglFSCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/universal.fs");
    #endif
}

template<>
void addToDescriptor<FragmentShader::baseTexture>(gfx::GraphicPipeline::Descriptor& descriptor)
{
    #ifdef GFX_METAL_ENABLED
        descriptor.metalFSFunction = "baseTexture_fs";
    #endif 
    #if GFX_OPENGL_ENABLED
        descriptor.openglFSCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/baseTexture.fs");
    #endif
}

template<>
void setVpMatrix<VertexShader::universal3D>(gfx::GraphicAPI& api, const math::mat4x4& mat)
{
    api.setVertexUniform("u_vpMatrix", mat);
}

template<>
void setModelMatrix<VertexShader::universal3D>(gfx::GraphicAPI& api, const math::mat4x4& mat)
{
    api.setVertexUniform("u_modelMatrix", mat);
}

template<>
void setCameraPos<FragmentShader::universal>(gfx::GraphicAPI& api, const math::vec3f& pos)
{
    api.setFragmentUniform("u_cameraPos", pos);
}

template<>
void setCameraPos<FragmentShader::baseColor>(gfx::GraphicAPI& api, const math::vec3f& pos)
{
    api.setFragmentUniform("u_cameraPos", pos);
}

template<>
void setCameraPos<FragmentShader::baseTexture>(gfx::GraphicAPI& api, const math::vec3f& pos)
{
    api.setFragmentUniform("u_cameraPos", pos);
}

template<>
void setPointLights<FragmentShader::universal>(gfx::GraphicAPI& api, const utils::Array<const PointLight*>& lights)
{
    utils::Array<shaderStruct::PointLight> pointLights;
    for (auto& light : lights) {
        pointLights.append({
            light->position,
            light->color,
            light->ambiantIntensity,
            light->diffuseIntensity,
            light->specularIntensity
        });
    }
    api.setFragmentUniform("u_pointLights", pointLights);
    api.setFragmentUniform("u_pointLightsCount", (utils::uint32)pointLights.length());
}

template<>
void setPointLights<FragmentShader::baseColor>(gfx::GraphicAPI& api, const utils::Array<const PointLight*>& lights)
{
    utils::Array<shaderStruct::PointLight> pointLights;
    for (auto& light : lights) {
        pointLights.append({
            light->position,
            light->color,
            light->ambiantIntensity,
            light->diffuseIntensity,
            light->specularIntensity
        });
    }
    api.setFragmentUniform("u_pointLights", pointLights);
    api.setFragmentUniform("u_pointLightsCount", (utils::uint32)pointLights.length());
}

template<>
void setPointLights<FragmentShader::baseTexture>(gfx::GraphicAPI& api, const utils::Array<const PointLight*>& lights)
{
    utils::Array<shaderStruct::PointLight> pointLights;
    for (auto& light : lights) {
        pointLights.append({
            light->position,
            light->color,
            light->ambiantIntensity,
            light->diffuseIntensity,
            light->specularIntensity
        });
    }
    api.setFragmentUniform("u_pointLights", pointLights);
    api.setFragmentUniform("u_pointLightsCount", (utils::uint32)pointLights.length());
}

template<>
void setMaterial<FragmentShader::universal>(gfx::GraphicAPI& api, const Material& mat)
{
    shaderStruct::universal::Material material;

    material.useAmbientTexture = mat.ambientTexture == true;
    if (material.useAmbientTexture)
        api.setFragmentTexture("u_ambientTexture", mat.ambientTexture);
    else
        material.ambientColor = mat.ambientColor;

    material.useDiffuseTexture = mat.diffuseTexture == true;
    if (material.useDiffuseTexture)
    {
        api.setFragmentTexture("u_diffuseTexture", mat.diffuseTexture);
        if (material.useAmbientTexture == false)
        {
            api.setFragmentTexture("u_ambientTexture", mat.diffuseTexture);
            material.useAmbientTexture = true;
        }
    }
    else
        material.diffuseColor = mat.diffuseColor;

    material.useSpecularTexture = mat.specularTexture == true;
    if (material.useSpecularTexture)
        api.setFragmentTexture("u_specularTexture", mat.specularTexture);
    else
        material.specularColor = mat.specularColor;

    material.useEmissiveTexture = mat.emissiveTexture == true;
    if (material.useEmissiveTexture)
        api.setFragmentTexture("u_emissiveTexture", mat.emissiveTexture);
    else
        material.emissiveColor = mat.emissiveColor;

    material.useShininessTexture = mat.shininessTexture == true;
    if (material.useShininessTexture)
        api.setFragmentTexture("u_shininessTexture", mat.shininessTexture);
    else
        material.shininess = mat.shininess;
        
    api.setFragmentUniform("u_material", material);
}

template<>
void setMaterial<FragmentShader::baseColor>(gfx::GraphicAPI& api, const Material& mat)
{
    shaderStruct::baseColor::Material material;
    material.baseColor = mat.diffuseColor;
    material.specularColor = mat.specularColor;
    material.emissiveColor = mat.emissiveColor;
    material.shininess = mat.shininess;
    api.setFragmentUniform("u_material", material);
}

template<>
void setMaterial<FragmentShader::baseTexture>(gfx::GraphicAPI& api, const Material& mat)
{
    shaderStruct::baseTexture::Material material;
    material.specularColor = mat.specularColor;
    material.emissiveColor = mat.emissiveColor;
    material.shininess = mat.shininess;
    api.setFragmentUniform("u_material", material);
    api.setFragmentTexture("u_diffuseTexture", mat.diffuseTexture);
}