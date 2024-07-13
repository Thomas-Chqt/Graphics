/*
 * ---------------------------------------------------
 * Renderer.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/06 19:22:45
 * ---------------------------------------------------
 */

#include "Renderer.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexLayout.hpp"
#include "Math/Constants.hpp"
#include "Math/Matrix.hpp"
#include "Mesh.hpp"
#include "UtilsCPP/Func.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"
#include <cstddef>

struct MatrixBuffer
{
    math::mat4x4 vpMatrix;
    math::mat4x4 modelMatrix;
};

Renderer::Renderer(const utils::SharedPtr<gfx::GraphicAPI>& api, const utils::SharedPtr<gfx::Window>& window) : m_window(window), m_graphicAPI(api)
{
    utils::Func<void(gfx::Event&)> updateVPMatrix = [this](gfx::Event& event) {
        event.dispatch<gfx::WindowResizeEvent>([this](gfx::WindowResizeEvent& event) {
            float fov = 60 * (PI / 180.0F);
            float aspectRatio = (float)event.width() / (float)event.height();
            float zNear = 0.1F;
            float zFar = 10000;

            float ys = 1.0F / std::tanf(fov * 0.5F);
            float xs = ys / aspectRatio;
            float zs = zFar / (zFar - zNear);
            
            math::mat4x4 projectionMatrix = math::mat4x4(xs,   0,  0,           0,
                                                          0,  ys,  0,           0,
                                                          0,   0, zs, -zNear * zs,
                                                          0,   0,  1,           0);
            math::mat4x4 viewMatrix = math::mat4x4::translation({0, 0, -3}).inversed();
            MatrixBuffer& matrices = *(MatrixBuffer*)m_matrixBuffer->mapContent();
            matrices.vpMatrix = projectionMatrix * viewMatrix;
            m_matrixBuffer->unMapContent();
        });
    };

    gfx::Shader::Descriptor shaderDescriptor;
    shaderDescriptor.type = gfx::ShaderType::vertex;
    #ifdef GFX_BUILD_METAL
        shaderDescriptor.mtlShaderLibPath = MTL_SHADER_LIB;
        shaderDescriptor.mtlFunction = "universal3D";
    #endif
    #ifdef GFX_BUILD_OPENGL
        shaderDescriptor.openglCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/universal3D.vs");
    #endif
    utils::SharedPtr<gfx::Shader> vs = m_graphicAPI->newShader(shaderDescriptor);
    shaderDescriptor.type = gfx::ShaderType::fragment;
    #ifdef GFX_BUILD_METAL
        shaderDescriptor.mtlShaderLibPath = MTL_SHADER_LIB;
        shaderDescriptor.mtlFunction = "phong1";
    #endif
    #ifdef GFX_BUILD_OPENGL
        shaderDescriptor.openglCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/phong1.fs");
    #endif
    utils::SharedPtr<gfx::Shader> fs = m_graphicAPI->newShader(shaderDescriptor);

    gfx::VertexLayout vertexLayout;
    vertexLayout.attributes.append({gfx::VertexAttributeFormat::vec3f, offsetof(Vertex, pos)});
    vertexLayout.attributes.append({gfx::VertexAttributeFormat::vec2f, offsetof(Vertex, uv)});
    vertexLayout.attributes.append({gfx::VertexAttributeFormat::vec3f, offsetof(Vertex, normal)});
    vertexLayout.stride = sizeof(Vertex);

    gfx::GraphicPipeline::Descriptor graphicPipelineDescriptor;
    graphicPipelineDescriptor.vertexLayout = vertexLayout;
    graphicPipelineDescriptor.vertexShader = vs;
    graphicPipelineDescriptor.fragmentShader = fs;

    m_defaultPipeline = m_graphicAPI->newGraphicsPipeline(graphicPipelineDescriptor);
    
    gfx::Buffer::Descriptor bufferDescriptor;
    bufferDescriptor.debugName = "Matrices";
    bufferDescriptor.size = sizeof(MatrixBuffer);

    m_matrixBuffer = m_graphicAPI->newBuffer(bufferDescriptor);

    utils::uint32 w = 0;
    utils::uint32 h = 0;
    m_window->getWindowSize(&w, &h);
    gfx::WindowResizeEvent ev(*m_window, (int)w, (int)h);
    updateVPMatrix(ev);
    m_window->addEventCallBack(updateVPMatrix, this);
}

void Renderer::render(const Mesh& mesh)
{
    m_graphicAPI->useGraphicsPipeline(m_defaultPipeline);
    m_graphicAPI->setVertexBuffer(m_matrixBuffer, m_defaultPipeline->getVertexBufferIndex("matrices"));

    utils::Func<void(const SubMesh&, const math::mat4x4&)> renderSubMesh = [&](const SubMesh& subMesh, const math::mat4x4& transform) {
        math::mat4x4 modelMatrix = transform * subMesh.transform;
        if (subMesh.vertexBuffer && subMesh.indexBuffer)
        {
            MatrixBuffer& matrices = *(MatrixBuffer*)m_matrixBuffer->mapContent();
            matrices.modelMatrix = modelMatrix;
            m_matrixBuffer->unMapContent();

            m_graphicAPI->setVertexBuffer(subMesh.vertexBuffer, 0);
            m_graphicAPI->drawIndexedVertices(subMesh.indexBuffer);
        }
        for (const auto& subMesh : subMesh.childs)
            renderSubMesh(subMesh, modelMatrix);
    };
    for (const auto& subMesh : mesh.subMeshes)
        renderSubMesh(subMesh, math::mat4x4(1.0F));    
}

Renderer::~Renderer()
{
    m_window->clearCallbacks(this);
}
