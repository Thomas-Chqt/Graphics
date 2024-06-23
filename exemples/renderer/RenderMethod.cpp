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

    api.setVertexUniform("u_modelMatrix", *data.modelMatrix);
    api.setVertexUniform("u_vpMatrix", *data.vpMatrix);
    api.setFragmentUniform("u_cameraPos", *data.cameraPos);
    api.setFragmentUniform("u_light", *data.pointLights);
    api.setFragmentUniform("u_material", *data.material);
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

    api.setVertexUniform("u_modelMatrix", *data.modelMatrix);
    api.setVertexUniform("u_vpMatrix", *data.vpMatrix);
    api.setFragmentUniform("u_material", *data.material);
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

    api.setVertexUniform("u_modelMatrix", *data.modelMatrix);
    api.setVertexUniform("u_vpMatrix", *data.vpMatrix);
    api.setFragmentUniform("u_cameraPos", *data.cameraPos);
    api.setFragmentUniform("u_light", *data.pointLights);
    api.setFragmentUniform("u_material", *data.material);
    api.setFragmentTexture("u_diffuseTexture", *data.diffuseTexture);
}


