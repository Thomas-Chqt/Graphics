/*
 * ---------------------------------------------------
 * RenderMethod.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/18 09:56:30
 * ---------------------------------------------------
 */

#include "RenderMethod.hpp"
#include "Graphics/BufferInstance.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include <utility>
#include "Renderer.hpp"

RenderMethod::RenderMethod(utils::SharedPtr<gfx::GraphicAPI> api, utils::SharedPtr<gfx::GraphicPipeline> pipeline)
    : m_graphicAPI(std::move(api)), m_graphicPipeline(std::move(pipeline))
{
}

utils::SharedPtr<gfx::GraphicPipeline> makeDefaultGraphicPipeline(const gfx::GraphicAPI& api)
{
    utils::SharedPtr<gfx::Shader> vs;
    {
        gfx::Shader::Descriptor shaderDescriptor;
        shaderDescriptor.type = gfx::ShaderType::vertex;
        #ifdef GFX_BUILD_METAL
            shaderDescriptor.mtlShaderLibPath = MTL_SHADER_LIB;
            shaderDescriptor.mtlFunction = "default_vs";
        #endif
        #ifdef GFX_BUILD_OPENGL
            shaderDescriptor.openglCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/default.vs");
        #endif
        vs = api.newShader(shaderDescriptor);
    }
    utils::SharedPtr<gfx::Shader> fs;
    {
        gfx::Shader::Descriptor shaderDescriptor;
        shaderDescriptor.type = gfx::ShaderType::fragment;
        #ifdef GFX_BUILD_METAL
            shaderDescriptor.mtlShaderLibPath = MTL_SHADER_LIB;
            shaderDescriptor.mtlFunction = "default_fs";
        #endif
        #ifdef GFX_BUILD_OPENGL
            shaderDescriptor.openglCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/default.fs");
        #endif
        fs = api.newShader(shaderDescriptor);
    }

    gfx::VertexLayout vertexLayout;
    vertexLayout.attributes.append({gfx::VertexAttributeFormat::vec3f, offsetof(DefaultRenderMethod::Vertex, pos)});
    vertexLayout.attributes.append({gfx::VertexAttributeFormat::vec2f, offsetof(DefaultRenderMethod::Vertex, uv)});
    vertexLayout.attributes.append({gfx::VertexAttributeFormat::vec3f, offsetof(DefaultRenderMethod::Vertex, normal)});
    vertexLayout.attributes.append({gfx::VertexAttributeFormat::vec3f, offsetof(DefaultRenderMethod::Vertex, tangent)});
    vertexLayout.stride = sizeof(DefaultRenderMethod::Vertex);

    gfx::GraphicPipeline::Descriptor graphicPipelineDescriptor;
    graphicPipelineDescriptor.vertexLayout = vertexLayout;
    graphicPipelineDescriptor.vertexShader = vs;
    graphicPipelineDescriptor.fragmentShader = fs;

    return api.newGraphicsPipeline(graphicPipelineDescriptor);
}

DefaultRenderMethod::DefaultRenderMethod(const utils::SharedPtr<gfx::GraphicAPI>& api)
    : RenderMethod(api, makeDefaultGraphicPipeline(*api)),
      m_materialBuffer(gfx::BufferInstance<MaterialBuffer>::alloc(m_graphicAPI))
{
    m_matrixBufferIdx = m_graphicPipeline->getVertexBufferIndex("matrices");
    m_lightBufferIdx = m_graphicPipeline->getFragmentBufferIndex("lights");
    m_cameraBufferIdx = m_graphicPipeline->getFragmentBufferIndex("camera");
    m_materialBufferIdx = m_graphicPipeline->getFragmentBufferIndex("material");
}

void DefaultRenderMethod::use()
{
    m_graphicAPI->useGraphicsPipeline(m_graphicPipeline);
    m_graphicAPI->setFragmentBuffer(m_materialBuffer.buffer(), m_materialBufferIdx);
}

void DefaultRenderMethod::setMatrixBuffer(const utils::SharedPtr<gfx::Buffer>& buff)
{
    m_graphicAPI->setVertexBuffer(buff, m_matrixBufferIdx);
}

void DefaultRenderMethod::setLightBuffer(const utils::SharedPtr<gfx::Buffer>& buff)
{
    m_graphicAPI->setFragmentBuffer(buff, m_lightBufferIdx);
}

void DefaultRenderMethod::setCameraBuffer(const utils::SharedPtr<gfx::Buffer>& buff)
{
    m_graphicAPI->setFragmentBuffer(buff, m_cameraBufferIdx);
}

void DefaultRenderMethod::setMaterial(const Material& material)
{
    m_materialBuffer.map();
    {
        m_materialBuffer.content().diffuseColor      = material.diffuse.value;
        m_materialBuffer.content().specularColor     = material.specular.value;
        m_materialBuffer.content().emissiveColor     = material.emissive.value;
        m_materialBuffer.content().shininess         = material.shininess;
        m_materialBuffer.content().useDiffuseTexture = material.diffuse.texture  ? 1 : 0;
        m_materialBuffer.content().useNormalMap      = material.normalMap        ? 1 : 0;
        m_materialBuffer.content().useSpecularMap    = material.specular.texture ? 1 : 0;
        m_materialBuffer.content().useEmissiveMap    = material.emissive.texture ? 1 : 0;

        if (m_materialBuffer.content().useDiffuseTexture != 0)
            m_graphicAPI->setFragmentTexture(material.diffuse.texture, m_graphicPipeline->getFragmentTextureIndex("diffuseTexture"));
        if (m_materialBuffer.content().useNormalMap != 0)
            m_graphicAPI->setFragmentTexture(material.normalMap, m_graphicPipeline->getFragmentTextureIndex("normalMap"));
        if (m_materialBuffer.content().useSpecularMap != 0)
            m_graphicAPI->setFragmentTexture(material.specular.texture, m_graphicPipeline->getFragmentTextureIndex("specularMap"));
        if (m_materialBuffer.content().useEmissiveMap != 0)
            m_graphicAPI->setFragmentTexture(material.emissive.texture, m_graphicPipeline->getFragmentTextureIndex("emissiveMap"));
    }
    m_materialBuffer.unmap();
}