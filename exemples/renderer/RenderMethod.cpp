/*
 * ---------------------------------------------------
 * RenderMethod.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/25 12:08:14
 * ---------------------------------------------------
 */

#include "RenderMethodLibrary.hpp"
#include "ShaderStructs.hpp"
#include "Entity.hpp"
#include "MaterialLibrary.hpp"
#include "UtilsCPP/Types.hpp"

template<>
gfx::StructLayout gfx::getLayout<shaderStruct::PointLight>()
{
    return {
        { "position",          Type::vec3f, (void*)offsetof(shaderStruct::PointLight, position)          },
        { "color",             Type::vec3f, (void*)offsetof(shaderStruct::PointLight, color)             },
        { "ambiantIntensity",  Type::Float, (void*)offsetof(shaderStruct::PointLight, ambiantIntensity)  },
        { "diffuseIntensity",  Type::Float, (void*)offsetof(shaderStruct::PointLight, diffuseIntensity)  },
        { "specularIntensity", Type::Float, (void*)offsetof(shaderStruct::PointLight, specularIntensity) }
    };
}

template<>
gfx::StructLayout gfx::getLayout<shaderStruct::baseTexture::Material>()
{
    return {
        { "specularColor", Type::vec3f, (void*)offsetof(shaderStruct::baseTexture::Material, specularColor)  },
        { "emissiveColor", Type::vec3f, (void*)offsetof(shaderStruct::baseTexture::Material, emissiveColor)  },
        { "shininess",     Type::Float, (void*)offsetof(shaderStruct::baseTexture::Material, shininess)      }
    };
}

template<>
gfx::StructLayout gfx::getLayout<shaderStruct::baseColor::Material>()
{
    return {
        { "baseColor",     Type::vec3f, (void*)offsetof(shaderStruct::baseColor::Material, baseColor)      },
        { "specularColor", Type::vec3f, (void*)offsetof(shaderStruct::baseColor::Material, specularColor)  },
        { "emissiveColor", Type::vec3f, (void*)offsetof(shaderStruct::baseColor::Material, emissiveColor)  },
        { "shininess",     Type::Float, (void*)offsetof(shaderStruct::baseColor::Material, shininess)      }
    };
}

template<>
void addToDescriptor<Shader::universal3D>(gfx::GraphicPipeline::Descriptor& descriptor)
{
    #ifdef GFX_METAL_ENABLED
        descriptor.metalVSFunction = "universal3D";
    #endif 
    #if GFX_OPENGL_ENABLED
        descriptor.openglVSCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/universal3D.vs");
    #endif
}

template<>
void addToDescriptor<Shader::baseTexture>(gfx::GraphicPipeline::Descriptor& descriptor)
{
    #ifdef GFX_METAL_ENABLED
        descriptor.metalFSFunction = "baseTexture_fs";
    #endif 
    #if GFX_OPENGL_ENABLED
        descriptor.openglFSCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/baseTexture.fs");
    #endif
}

template<>
void addToDescriptor<Shader::baseColor>(gfx::GraphicPipeline::Descriptor& descriptor)
{
    #ifdef GFX_METAL_ENABLED
        descriptor.metalFSFunction = "baseColor_fs";
    #endif 
    #if GFX_OPENGL_ENABLED
        descriptor.openglFSCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/baseColor.fs");
    #endif
}

template<>
void setVpMatrix<Shader::universal3D>(gfx::GraphicAPI& api, const math::mat4x4& mat)
{
    api.setVertexUniform("u_vpMatrix", mat);
}




template<>
void setUniforms<Shader::universal3D>(gfx::GraphicAPI& api, const IRenderMethod::Uniforms& uniforms)
{
    api.setVertexUniform("u_modelMatrix", uniforms.modelMatrix);
    api.setVertexUniform("u_vpMatrix",    uniforms.vpMatrix);
}

template<>
void setUniforms<Shader::baseTexture>(gfx::GraphicAPI& api, const IRenderMethod::Uniforms& uniforms)
{
    using shaderStruct::PointLight;
    using shaderStruct::baseTexture::Material;

    utils::Array<PointLight> lights;
    for (auto& light : uniforms.pointLights) {
        lights.append({
            light->position,
            light->color,
            light->ambiantIntensity,
            light->diffuseIntensity,
            light->specularIntensity
        });
    }

    Material material;
    material.specularColor = uniforms.material.specularColor;
    material.emissiveColor = uniforms.material.emissiveColor;
    material.shininess = uniforms.material.shininess;

    api.setFragmentUniform("u_cameraPos", uniforms.cameraPos);
    api.setFragmentUniform("u_pointLights", lights);
    api.setFragmentUniform("u_pointLightsCount", (utils::uint32)lights.length());
    api.setFragmentUniform("u_material", material);

    if (uniforms.material.baseTexture)
        api.setFragmentTexture("u_diffuseTexture", uniforms.material.baseTexture);
}

template<>
void setUniforms<Shader::baseColor>(gfx::GraphicAPI& api, const IRenderMethod::Uniforms& uniforms)
{
    using shaderStruct::PointLight;
    using shaderStruct::baseColor::Material;

    utils::Array<PointLight> lights;
    for (auto& light : uniforms.pointLights) {
        lights.append({
            light->position,
            light->color,
            light->ambiantIntensity,
            light->diffuseIntensity,
            light->specularIntensity
        });
    }

    Material material;
    material.baseColor = uniforms.material.baseColor;
    material.specularColor = uniforms.material.specularColor;
    material.emissiveColor = uniforms.material.emissiveColor;
    material.shininess = uniforms.material.shininess;

    api.setFragmentUniform("u_cameraPos", uniforms.cameraPos);
    api.setFragmentUniform("u_pointLights", lights);
    api.setFragmentUniform("u_pointLightsCount", (utils::uint32)lights.length());
    api.setFragmentUniform("u_material", material);
}