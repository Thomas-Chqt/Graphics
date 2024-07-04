/*
 * ---------------------------------------------------
 * Renderer.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/21 10:37:45
 * ---------------------------------------------------
 */

#include "Renderer.hpp"
#include "AssetManager.hpp"
#include "Entity.hpp"
#include "Graphics/Event.hpp"
#include "Graphics/Window.hpp"
#include "Math/Constants.hpp"
#include "Math/Matrix.hpp"
#include "RenderMethod.hpp"
#include "UtilsCPP/Array.hpp"
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

void Renderer::beginScene()
{
}

void Renderer::setCamera(const Camera& camera)
{
    m_camera = &camera;
}

void Renderer::addPointLight(const PointLight& light)
{
    m_pointLights.append(&light);
}

void Renderer::addMesh(const Mesh& mesh, const math::mat4x4& transformMatrix)
{
    utils::Func<void(const SubMesh&, const math::mat4x4&)> addSubMesh = [&](const SubMesh& subMesh, const math::mat4x4& parentTransform) {
        SubMesh worldSpaceSubMesh = subMesh;
        worldSpaceSubMesh.transform = parentTransform * subMesh.transform;
        if (subMesh.vertexBuffer)
            m_renderables.get(subMesh.material).append(worldSpaceSubMesh);
        for (auto& child : subMesh.childs)
            addSubMesh(child, worldSpaceSubMesh.transform);
    };

    for (auto& subMesh : mesh.subMeshes)
        addSubMesh(subMesh, transformMatrix);
}

void Renderer::endScene()
{
    m_api->beginFrame();
    m_api->beginOnScreenRenderPass();

    math::mat4x4 vpMatrix = m_projectionMatrix * m_camera->viewMatrix();

    for (auto& renderable : m_renderables)
    {
        IRenderMethod& renderMethod = *renderable.key->renderMethod;
        renderMethod.use();
        renderMethod.setVpMatrix(m_projectionMatrix * m_camera->viewMatrix());
        renderMethod.setCameraPos(m_camera->position);
        renderMethod.setPointLights(m_pointLights);
        renderMethod.setMaterial(*renderable.key);

        for (auto& mesh : renderable.val) {
            renderMethod.setModelMatrix(mesh.transform);
            m_api->useVertexBuffer(mesh.vertexBuffer);
            m_api->drawIndexedVertices(mesh.indexBuffer);
        }
    }

    m_makeUI();

    m_api->endOnScreenRenderPass();
    m_api->endFrame();

    m_renderables.clear();
    m_pointLights.clear();
}

Renderer::~Renderer()
{
    m_window->clearCallbacks(this);
}