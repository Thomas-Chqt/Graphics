/*
 * ---------------------------------------------------
 * Renderer.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/21 10:37:45
 * ---------------------------------------------------
 */

#include "Renderer.hpp"
#include "Graphics/Event.hpp"
#include "Graphics/Window.hpp"
#include "Math/Constants.hpp"
#include "UtilsCPP/Func.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/Types.hpp"
#include <cmath>

Renderer::Renderer(const utils::SharedPtr<gfx::Window>& window, const utils::SharedPtr<gfx::GraphicAPI>& api) : m_window(window), m_api(api)
{
    utils::Func<void(gfx::Event&)> updateProjectionMatrix = [this](gfx::Event& event) {
        event.dispatch<gfx::WindowResizeEvent>([this](gfx::WindowResizeEvent& event) {
            float fov = 60 * (PI / 180.0f);
            float aspectRatio = (float)event.width() / (float)event.height();
            float zNear = 0.1f;
            float zFar = 10000;

            float ys = 1 / std::tan(fov * 0.5);
            float xs = ys / aspectRatio;
            float zs = zFar / (zFar - zNear);

            m_projectionMatrix = math::mat4x4(xs,   0,  0,           0,
                                               0,  ys,  0,           0,
                                               0,   0, zs, -zNear * zs,
                                               0,   0,  1,           0);
        });
    };

    utils::uint32 w, h;
    window->getWindowSize(&w, &h);
    gfx::WindowResizeEvent ev(*window, w, h);
    updateProjectionMatrix(ev);
    window->addEventCallBack(updateProjectionMatrix, this);
}

void Renderer::render(RenderableEntity& entt)
{
    for (auto& subMesh : entt.subMeshes)
    {
        m_api->useGraphicsPipeline(subMesh.pipeline);
        
        m_api->setVertexUniform(subMesh.pipeline->findVertexUniformIndex("u_modelMatrix"), entt.modelMatrix());
        m_api->setVertexUniform(subMesh.pipeline->findVertexUniformIndex("u_vpMatrix"), m_projectionMatrix * m_camera->viewMatrix());

        m_api->setFragmentUniform(subMesh.pipeline->findFragmentUniformIndex("u_cameraPos"), m_camera->position);
        if (m_directionalLight)
        {
            m_api->setFragmentUniform(subMesh.pipeline->findFragmentUniformIndex("u_light_direction"), m_directionalLight->direction);
            m_api->setFragmentUniform(subMesh.pipeline->findFragmentUniformIndex("u_light_color"), m_directionalLight->color);
            m_api->setFragmentUniform(subMesh.pipeline->findFragmentUniformIndex("u_light_ambiantIntensity"), m_directionalLight->ambiantIntensity);
            m_api->setFragmentUniform(subMesh.pipeline->findFragmentUniformIndex("u_light_diffuseIntensity"), m_directionalLight->diffuseIntensity);
            m_api->setFragmentUniform(subMesh.pipeline->findFragmentUniformIndex("u_light_specularIntensity"), m_directionalLight->specularIntensity);
        }

        m_api->setFragmentUniform(subMesh.pipeline->findFragmentUniformIndex("u_material_ambiant"), subMesh.material.ambiant);
        m_api->setFragmentUniform(subMesh.pipeline->findFragmentUniformIndex("u_material_diffuse"), subMesh.material.diffuse);
        m_api->setFragmentUniform(subMesh.pipeline->findFragmentUniformIndex("u_material_specular"), subMesh.material.specular);
        m_api->setFragmentUniform(subMesh.pipeline->findFragmentUniformIndex("u_material_shininess"), subMesh.material.shininess);

        m_api->useVertexBuffer(subMesh.vertexBuffer);
        m_api->drawIndexedVertices(subMesh.indexBuffer);
    }
}

Renderer::~Renderer()
{
    m_window->clearCallbacks(this);
}