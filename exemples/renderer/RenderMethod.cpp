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
RenderMethod<Shader::universal3D, Shader::baseColor>::RenderMethod(const utils::SharedPtr<gfx::GraphicAPI>& api)
    : m_graphicAPI(api)
{
    gfx::GraphicPipeline::Descriptor descriptor;

    #ifdef GFX_METAL_ENABLED
        descriptor.metalVSFunction = "universal3D";
        descriptor.metalFSFunction = "baseColor_fs";
    #endif 
    #if GFX_OPENGL_ENABLED
        descriptor.openglVSCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/universal3D.vs");
        descriptor.openglFSCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/baseColor.fs");
    #endif

    m_graphicPipeline = m_graphicAPI->newGraphicsPipeline(descriptor);
}

template<>
void RenderMethod<Shader::universal3D, Shader::baseColor>::setVpMatrix(const math::mat4x4& mat)
{
    m_graphicAPI->setVertexUniform("u_vpMatrix", mat);
}

template<>
void RenderMethod<Shader::universal3D, Shader::baseColor>::setModelMatrix(const math::mat4x4& mat)
{
    m_graphicAPI->setVertexUniform("u_modelMatrix", mat);
}

template<>
void RenderMethod<Shader::universal3D, Shader::baseColor>::setCameraPos(const math::vec3f& pos)
{
    m_graphicAPI->setFragmentUniform("u_cameraPos", pos);
}

template<>
void RenderMethod<Shader::universal3D, Shader::baseColor>::setPointLights(const utils::Array<const PointLight*>& lights)
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
    m_graphicAPI->setFragmentUniform("u_pointLights", pointLights);
    m_graphicAPI->setFragmentUniform("u_pointLightsCount", (utils::uint32)pointLights.length());
}

template<>
void RenderMethod<Shader::universal3D, Shader::baseColor>::setMaterial(const Material& mat)
{
    shaderStruct::baseColor::Material material;
    material.baseColor = mat.baseColor;
    material.specularColor = mat.specularColor;
    material.emissiveColor = mat.emissiveColor;
    material.shininess = mat.shininess;
    m_graphicAPI->setFragmentUniform("u_material", material);
}