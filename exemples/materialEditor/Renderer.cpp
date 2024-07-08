/*
 * ---------------------------------------------------
 * Renderer.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/06 19:22:45
 * ---------------------------------------------------
 */

#include "Renderer.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Math/Constants.hpp"
#include "Math/Matrix.hpp"
#include "Mesh.hpp"
#include "UtilsCPP/Func.hpp"

Renderer::Renderer(const utils::SharedPtr<gfx::GraphicAPI>& api, const utils::SharedPtr<gfx::Window>& window) : m_window(window), m_graphicAPI(api)
{
    utils::Func<void(gfx::Event&)> updateVPMatrix = [this](gfx::Event& event) {
        event.dispatch<gfx::WindowResizeEvent>([this](gfx::WindowResizeEvent& event) {
            float fov = 60 * (PI / 180.0f);
            float aspectRatio = (float)event.width() / (float)event.height();
            float zNear = 0.1f;
            float zFar = 10000;

            float ys = 1 / std::tan(fov * 0.5);
            float xs = ys / aspectRatio;
            float zs = zFar / (zFar - zNear);
            
            math::mat4x4 projectionMatrix = math::mat4x4(xs,   0,  0,           0,
                                                          0,  ys,  0,           0,
                                                          0,   0, zs, -zNear * zs,
                                                          0,   0,  1,           0);
            math::mat4x4 viewMatrix = math::mat4x4::translation({0, 0, -3}).inversed();
            m_vpMatrix = projectionMatrix * viewMatrix;
        });
    };

    utils::uint32 w, h;
    m_window->getWindowSize(&w, &h);
    gfx::WindowResizeEvent ev(*m_window, w, h);
    updateVPMatrix(ev);
    m_window->addEventCallBack(updateVPMatrix, this);

    #ifdef GFX_METAL_ENABLED
        m_graphicAPI->initMetalShaderLib(MTL_SHADER_LIB);
    #endif

    gfx::GraphicPipeline::Descriptor graphicPipelineDescriptor;
    #ifdef GFX_METAL_ENABLED
        graphicPipelineDescriptor.metalVSFunction = "universal3D";
        graphicPipelineDescriptor.metalFSFunction = "phong1";
    #endif
    m_defaultPipeline = m_graphicAPI->newGraphicsPipeline(graphicPipelineDescriptor);
}

void Renderer::render(const Mesh& mesh, const DirectionalLight& light, const Material& material)
{
    m_graphicAPI->useGraphicsPipeline(m_defaultPipeline);
    m_graphicAPI->setVertexUniform("u_vpMatrix", m_vpMatrix);

    // m_graphicAPI->setFragmentUniform("u_light", light.getShaderShared());

    utils::Func<void(const SubMesh&, const math::mat4x4&)> renderSubMesh = [&](const SubMesh& subMesh, const math::mat4x4& transform){
        math::mat4x4 modelMatrix = transform * subMesh.transform;
        if (subMesh.vertexBuffer && subMesh.indexBuffer)
        {
            m_graphicAPI->setVertexUniform("u_modelMatrix", modelMatrix);
            m_graphicAPI->useVertexBuffer(subMesh.vertexBuffer);
            m_graphicAPI->drawIndexedVertices(subMesh.indexBuffer);
        }
        for (auto& subMesh : subMesh.childs)
            renderSubMesh(subMesh, modelMatrix);
    };
    for (auto& subMesh : mesh.subMeshes)
        renderSubMesh(subMesh, math::mat4x4(1.0f));    
}

Renderer::~Renderer()
{
    m_window->clearCallbacks(this);
}
