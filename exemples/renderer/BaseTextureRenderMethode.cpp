/*
 * ---------------------------------------------------
 * BaseTextureRenderMethode.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/25 13:43:44
 * ---------------------------------------------------
 */

#include "Math/Vector.hpp"
#include "RenderMethod.hpp"
#include "UtilsCPP/Array.hpp"

using ThisRenderMethod = RenderMethod<VertexShader::universal3D, FragmentShader::baseTexture>;

struct ThisRenderMethodLight
{
    math::vec3f position;
    math::rgb   color;
    float       ambiantIntensity;
    float       diffuseIntensity;
    float       specularIntensity;
};

template<>
gfx::StructLayout gfx::getLayout<ThisRenderMethodLight>()
{
    return {
        { "position",          Type::vec3f, (void*)offsetof(ThisRenderMethodLight, position)          },
        { "color",             Type::vec3f, (void*)offsetof(ThisRenderMethodLight, color)             },
        { "ambiantIntensity",  Type::Float, (void*)offsetof(ThisRenderMethodLight, ambiantIntensity)  },
        { "diffuseIntensity",  Type::Float, (void*)offsetof(ThisRenderMethodLight, diffuseIntensity)  },
        { "specularIntensity", Type::Float, (void*)offsetof(ThisRenderMethodLight, specularIntensity) }
    };
}

struct ThisRenderMethodMaterial
{
    math::rgb specularColor;
    math::rgb emissiveColor;
    float     shininess;
};

template<>
gfx::StructLayout gfx::getLayout<ThisRenderMethodMaterial>()
{
    return {
        { "specularColor", Type::vec3f, (void*)offsetof(ThisRenderMethodMaterial, specularColor)  },
        { "emissiveColor", Type::vec3f, (void*)offsetof(ThisRenderMethodMaterial, emissiveColor)  },
        { "shininess",     Type::Float, (void*)offsetof(ThisRenderMethodMaterial, shininess)      }
    };
}

template<>
ThisRenderMethod::RenderMethod(const utils::SharedPtr<gfx::GraphicAPI>& api) : m_graphicAPI(api)
{
    gfx::GraphicPipeline::Descriptor graphicPipelineDescriptor;

    #ifdef GFX_METAL_ENABLED
        graphicPipelineDescriptor.metalVSFunction = "universal_vs";
        graphicPipelineDescriptor.metalFSFunction = "universal_fs";
    #endif 
    #if GFX_OPENGL_ENABLED
        // graphicPipelineDescriptor.openglVSCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/universal.vs");
        // graphicPipelineDescriptor.openglFSCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/universal.fs");
    #endif

    m_graphicPipeline = m_graphicAPI->newGraphicsPipeline(graphicPipelineDescriptor);
}

template<>
void ThisRenderMethod::use(const Data& data)
{
    utils::Array<ThisRenderMethodLight> lights;
    for (auto& light : *data.pointLights) {
        lights.append({
            light.position,
            light.color,
            light.ambiantIntensity,
            light.diffuseIntensity,
            light.specularIntensity
        });
    }

    ThisRenderMethodMaterial material;
    // if (data.material->specularTexture == false)
        material.specularColor = data.material->specularColor;
    // if (data.material->emissiveTexture == false)
        material.emissiveColor = data.material->emissiveColor;
    material.shininess = data.material->shininess;

    m_graphicAPI->useGraphicsPipeline(m_graphicPipeline);
    
    m_graphicAPI->setVertexUniform("u_modelMatrix", *data.modelMatrix);
    m_graphicAPI->setVertexUniform("u_vpMatrix", *data.vpMatrix);

    m_graphicAPI->setFragmentUniform("u_cameraPos", *data.cameraPos);
    m_graphicAPI->setFragmentUniform("u_lights", lights);
    m_graphicAPI->setFragmentUniform("u_material", material);

    if (data.material->baseTexture)
        m_graphicAPI->setFragmentTexture("u_diffuseTexture", data.material->baseTexture);
}