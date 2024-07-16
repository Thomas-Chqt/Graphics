/*
 * ---------------------------------------------------
 * RenderMethod.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/15 10:39:28
 * ---------------------------------------------------
 */

#include "RenderMethod.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include <utility>

RenderMethod::RenderMethod(utils::SharedPtr<gfx::GraphicAPI> api, utils::SharedPtr<gfx::GraphicPipeline> pipeline)
    : m_api(std::move(api)), m_graphicPipeline(std::move(pipeline))
{
}

utils::SharedPtr<gfx::GraphicPipeline> makePhongGraphicPipeline(const gfx::GraphicAPI& api)
{
    gfx::Shader::Descriptor shaderDescriptor;
    shaderDescriptor.type = gfx::ShaderType::vertex;
    #ifdef GFX_BUILD_METAL
        shaderDescriptor.mtlShaderLibPath = MTL_SHADER_LIB;
        shaderDescriptor.mtlFunction = "universal3D";
    #endif
    #ifdef GFX_BUILD_OPENGL
        shaderDescriptor.openglCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/universal3D.vs");
    #endif
    utils::SharedPtr<gfx::Shader> vs = api.newShader(shaderDescriptor);
    shaderDescriptor.type = gfx::ShaderType::fragment;
    #ifdef GFX_BUILD_METAL
        shaderDescriptor.mtlShaderLibPath = MTL_SHADER_LIB;
        shaderDescriptor.mtlFunction = "phong1";
    #endif
    #ifdef GFX_BUILD_OPENGL
        shaderDescriptor.openglCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/phong1.fs");
    #endif
    utils::SharedPtr<gfx::Shader> fs = api.newShader(shaderDescriptor);

    gfx::VertexLayout vertexLayout;
    vertexLayout.attributes.append({gfx::VertexAttributeFormat::vec3f, offsetof(PhongRenderMethod::Vertex, pos)});
    vertexLayout.attributes.append({gfx::VertexAttributeFormat::vec2f, offsetof(PhongRenderMethod::Vertex, uv)});
    vertexLayout.attributes.append({gfx::VertexAttributeFormat::vec3f, offsetof(PhongRenderMethod::Vertex, normal)});
    vertexLayout.attributes.append({gfx::VertexAttributeFormat::vec3f, offsetof(PhongRenderMethod::Vertex, tangent)});
    vertexLayout.stride = sizeof(PhongRenderMethod::Vertex);

    gfx::GraphicPipeline::Descriptor graphicPipelineDescriptor;
    graphicPipelineDescriptor.vertexLayout = vertexLayout;
    graphicPipelineDescriptor.vertexShader = vs;
    graphicPipelineDescriptor.fragmentShader = fs;

    return api.newGraphicsPipeline(graphicPipelineDescriptor);
}

PhongRenderMethod::PhongRenderMethod(const utils::SharedPtr<gfx::GraphicAPI>& api) : RenderMethod(api, makePhongGraphicPipeline(*api))
{
    {
        gfx::Buffer::Descriptor bufferDescriptor;
        bufferDescriptor.debugName = "Matrices";
        bufferDescriptor.size = sizeof(Matrices);
        m_matrixBuffer = m_api->newBuffer(bufferDescriptor);
    }
    {
        gfx::Buffer::Descriptor bufferDescriptor;
        bufferDescriptor.debugName = "Material";
        bufferDescriptor.size = sizeof(Material);
        m_materialBuffer = m_api->newBuffer(bufferDescriptor);
    }
    {
        gfx::Buffer::Descriptor bufferDescriptor;
        bufferDescriptor.debugName = "Light";
        bufferDescriptor.size = sizeof(DirectionalLight);
        m_lightBuffer = m_api->newBuffer(bufferDescriptor);
    }
}

void PhongRenderMethod::use()
{
    m_api->useGraphicsPipeline(m_graphicPipeline);
    m_api->setVertexBuffer(m_matrixBuffer, m_graphicPipeline->getVertexBufferIndex("matrices"));
    m_api->setFragmentBuffer(m_materialBuffer, m_graphicPipeline->getFragmentBufferIndex("material"));
    m_api->setFragmentBuffer(m_lightBuffer, m_graphicPipeline->getFragmentBufferIndex("light"));
}

void PhongRenderMethod::setVpMatrix(math::mat4x4 mat)
{
    Matrices& matrices = *(Matrices*)m_matrixBuffer->mapContent();
    {
        matrices.vpMatrix = mat;
    }
    m_matrixBuffer->unMapContent();
}

void PhongRenderMethod::setModelMatrix(math::mat4x4 mat)
{
    Matrices& matrices = *(Matrices*)m_matrixBuffer->mapContent();
    {
        matrices.modelMatrix = mat;
    }
    m_matrixBuffer->unMapContent();
}

void PhongRenderMethod::setMaterial(const ::Material& material)
{
    auto& gpuMaterial = *(PhongRenderMethod::Material*)m_materialBuffer->mapContent();
    {
        gpuMaterial.diffuseColor = material.diffuse.value;
        gpuMaterial.specularColor = material.specular.value;
        gpuMaterial.emissiveColor = material.emissive.value;
        gpuMaterial.shininess = material.shininess;
        gpuMaterial.useDiffuseTexture = material.diffuse.texture ? 1 : 0;
        gpuMaterial.useNormalMap = material.normalMap ? 1 : 0;
        gpuMaterial.useSpecularMap = material.specular.texture ? 1 : 0;
        gpuMaterial.useEmissiveMap = material.emissive.texture ? 1 : 0;
    }
    m_materialBuffer->unMapContent();

    if (gpuMaterial.useDiffuseTexture != 0)
        m_api->setFragmentTexture(material.diffuse.texture, m_graphicPipeline->getFragmentTextureIndex("diffuseTexture"));
    if (gpuMaterial.useNormalMap != 0)
        m_api->setFragmentTexture(material.normalMap, m_graphicPipeline->getFragmentTextureIndex("normalMap"));
    if (gpuMaterial.useSpecularMap != 0)
        m_api->setFragmentTexture(material.specular.texture, m_graphicPipeline->getFragmentTextureIndex("specularMap"));
    if (gpuMaterial.useEmissiveMap != 0)
        m_api->setFragmentTexture(material.emissive.texture, m_graphicPipeline->getFragmentTextureIndex("emissiveMap"));
}

void PhongRenderMethod::setLight(const ::DirectionalLight& light)
{
    auto& gpuLight = *(PhongRenderMethod::DirectionalLight*)m_lightBuffer->mapContent();
    {
        gpuLight.color = light.color;
        gpuLight.ambiantIntensity = light.ambiantIntensity;
        gpuLight.diffuseIntensity = light.diffuseIntensity;
        gpuLight.specularIntensity = light.specularIntensity;
        gpuLight.direction = (math::vec4f({0, 0, 1, 0}) * math::mat4x4::rotation(light.rotation) * math::mat4x4::rotation(light.rotation)).xyz();
    }
    m_lightBuffer->unMapContent();
}

utils::SharedPtr<gfx::GraphicPipeline> makeSkyboxGraphicPipeline(const gfx::GraphicAPI& api)
{
    gfx::Shader::Descriptor shaderDescriptor;
    shaderDescriptor.type = gfx::ShaderType::vertex;
    #ifdef GFX_BUILD_METAL
        shaderDescriptor.mtlShaderLibPath = MTL_SHADER_LIB;
        shaderDescriptor.mtlFunction = "skybox_vs";
    #endif
    #ifdef GFX_BUILD_OPENGL
        shaderDescriptor.openglCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/skybox.vs");
    #endif
    utils::SharedPtr<gfx::Shader> vs = api.newShader(shaderDescriptor);
    shaderDescriptor.type = gfx::ShaderType::fragment;
    #ifdef GFX_BUILD_METAL
        shaderDescriptor.mtlShaderLibPath = MTL_SHADER_LIB;
        shaderDescriptor.mtlFunction = "skybox_fs";
    #endif
    #ifdef GFX_BUILD_OPENGL
        shaderDescriptor.openglCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/skybox.fs");
    #endif
    utils::SharedPtr<gfx::Shader> fs = api.newShader(shaderDescriptor);

    gfx::VertexLayout vertexLayout;
    vertexLayout.attributes.append({gfx::VertexAttributeFormat::vec3f, offsetof(SkyboxRenderMethod::Vertex, pos)});
    vertexLayout.stride = sizeof(PhongRenderMethod::Vertex);

    gfx::GraphicPipeline::Descriptor graphicPipelineDescriptor;
    graphicPipelineDescriptor.vertexLayout = vertexLayout;
    graphicPipelineDescriptor.vertexShader = vs;
    graphicPipelineDescriptor.fragmentShader = fs;

    return api.newGraphicsPipeline(graphicPipelineDescriptor);
}

SkyboxRenderMethod::SkyboxRenderMethod(const utils::SharedPtr<gfx::GraphicAPI>& api) : RenderMethod(api, makeSkyboxGraphicPipeline(*api))
{
    gfx::Buffer::Descriptor bufferDescriptor;
    bufferDescriptor.debugName = "Matrices";
    bufferDescriptor.size = sizeof(Matrices);
    m_matrixBuffer = m_api->newBuffer(bufferDescriptor);
}

void SkyboxRenderMethod::use()
{
    m_api->useGraphicsPipeline(m_graphicPipeline);
}

void SkyboxRenderMethod::setVpMatrix(math::mat4x4 mat)
{
    auto& matrices = *(SkyboxRenderMethod::Matrices*)m_matrixBuffer->mapContent();
    {
        matrices.vpMatrix = mat;
    }
    m_matrixBuffer->unMapContent();
}

void SkyboxRenderMethod::setTextureCube(const utils::SharedPtr<gfx::Texture>& texture)
{
    m_api->setFragmentTexture(texture, m_graphicPipeline->getFragmentTextureIndex("skyboxTexture"));
}