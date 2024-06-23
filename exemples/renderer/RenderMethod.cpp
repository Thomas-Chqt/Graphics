/*
 * ---------------------------------------------------
 * RenderMethod.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/22 13:14:40
 * ---------------------------------------------------
 */

#include "RenderMethod.hpp"

FlatColorRenderMethod::FlatColorRenderMethod(const gfx::GraphicAPI& api)
{
    gfx::GraphicPipeline::Descriptor graphicPipelineDescriptor;
    #ifdef GFX_METAL_ENABLED
        graphicPipelineDescriptor.metalVSFunction = "simpleVertex";
        graphicPipelineDescriptor.metalFSFunction = "flatColor_fs";
    #endif 
    #if GFX_OPENGL_ENABLED
        graphicPipelineDescriptor.openglVSCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/simpleVertex.vs");
        graphicPipelineDescriptor.openglFSCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/flatColor.fs");
    #endif

    m_pipeline = api.newGraphicsPipeline(graphicPipelineDescriptor);
}

void FlatColorRenderMethod::use(gfx::GraphicAPI& api, const Data& data)
{
    api.useGraphicsPipeline(m_pipeline);

    api.setVertexUniform(m_pipeline->findVertexUniformIndex("u_modelMatrix"), *data.modelMatrix);
    api.setVertexUniform(m_pipeline->findVertexUniformIndex("u_vpMatrix"),    *data.vpMatrix);

    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_cameraPos"), *data.cameraPos);

    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_light_position"), (*data.pointLights)[0].position);
    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_light_color"), (*data.pointLights)[0].color);
    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_light_ambiantIntensity"), (*data.pointLights)[0].ambiantIntensity);
    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_light_diffuseIntensity"), (*data.pointLights)[0].diffuseIntensity);
    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_light_specularIntensity"), (*data.pointLights)[0].specularIntensity);

    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_material_ambiant"), data.material->ambiant);
    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_material_diffuse"), data.material->diffuse);
    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_material_specular"), data.material->specular);
    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_material_emissive"), data.material->emissive);
    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_material_shininess"), data.material->shininess);
}

LightCubeRenderMethod::LightCubeRenderMethod(const gfx::GraphicAPI& api)
{
    gfx::GraphicPipeline::Descriptor graphicPipelineDescriptor;
    #ifdef GFX_METAL_ENABLED
        graphicPipelineDescriptor.metalVSFunction = "simpleVertex";
        graphicPipelineDescriptor.metalFSFunction = "lightCube_fs";
    #endif 
    #if GFX_OPENGL_ENABLED
        graphicPipelineDescriptor.openglVSCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/simpleVertex.vs");
        graphicPipelineDescriptor.openglFSCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/lightCube.fs");
    #endif

    m_pipeline = api.newGraphicsPipeline(graphicPipelineDescriptor);
}

void LightCubeRenderMethod::use(gfx::GraphicAPI& api, const Data& data)
{
    api.useGraphicsPipeline(m_pipeline);

    api.setVertexUniform(m_pipeline->findVertexUniformIndex("u_modelMatrix"), *data.modelMatrix);
    api.setVertexUniform(m_pipeline->findVertexUniformIndex("u_vpMatrix"),    *data.vpMatrix);

    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_material_ambiant"), data.material->ambiant);
    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_material_diffuse"), data.material->diffuse);
    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_material_specular"), data.material->specular);
    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_material_emissive"), data.material->emissive);
    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_material_shininess"), data.material->shininess);
}

TexturedRenderMethod::TexturedRenderMethod(const gfx::GraphicAPI& api)
{
    gfx::GraphicPipeline::Descriptor graphicPipelineDescriptor;
    #ifdef GFX_METAL_ENABLED
        graphicPipelineDescriptor.metalVSFunction = "simpleVertex";
        graphicPipelineDescriptor.metalFSFunction = "textured_fs";
    #endif 
    #if GFX_OPENGL_ENABLED
        graphicPipelineDescriptor.openglVSCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/simpleVertex.vs");
        graphicPipelineDescriptor.openglFSCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/textured.fs");
    #endif

    m_pipeline = api.newGraphicsPipeline(graphicPipelineDescriptor);
}

void TexturedRenderMethod::use(gfx::GraphicAPI& api, const Data& data)
{
    api.useGraphicsPipeline(m_pipeline);

    api.setVertexUniform(m_pipeline->findVertexUniformIndex("u_modelMatrix"), *data.modelMatrix);
    api.setVertexUniform(m_pipeline->findVertexUniformIndex("u_vpMatrix"),    *data.vpMatrix);

    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_cameraPos"), *data.cameraPos);

    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_light_position"), (*data.pointLights)[0].position);
    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_light_color"), (*data.pointLights)[0].color);
    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_light_ambiantIntensity"), (*data.pointLights)[0].ambiantIntensity);
    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_light_diffuseIntensity"), (*data.pointLights)[0].diffuseIntensity);
    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_light_specularIntensity"), (*data.pointLights)[0].specularIntensity);

    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_material_ambiant"), data.material->ambiant);
    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_material_diffuse"), data.material->diffuse);
    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_material_specular"), data.material->specular);
    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_material_emissive"), data.material->emissive);
    api.setFragmentUniform(m_pipeline->findFragmentUniformIndex("u_material_shininess"), data.material->shininess);

    api.setFragmentTexture(m_pipeline->findFragmentUniformIndex("u_diffuseTexture"), *data.diffuseTexture);
}


