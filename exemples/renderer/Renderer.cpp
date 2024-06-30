/*
 * ---------------------------------------------------
 * Renderer.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/21 10:37:45
 * ---------------------------------------------------
 */

#include "Renderer.hpp"
#include "Entity.hpp"
#include "Graphics/Event.hpp"
#include "Graphics/Window.hpp"
#include "Math/Constants.hpp"
#include "Math/Matrix.hpp"
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

void Renderer::addRenderableEntity(const RenderableEntity& entt)
{
    utils::Func<void(math::mat4x4, const RenderableEntity&)> recurse = [&](math::mat4x4 parentModelMatrix, const RenderableEntity& entity) {
        math::mat4x4 modelMatrix = parentModelMatrix * entity.modelMatrix();
        for (auto mesh : entity.meshes)
        {
            m_renderables.get(mesh.material, utils::Array<Renderable>()).append({
                modelMatrix,
                mesh.vertexBuffer,
                mesh.indexBuffer,
                mesh.material
            });
        }
        for (Entity* child : entity.subEntities)
        {
            if (RenderableEntity* rEntt = dynamic_cast<RenderableEntity*>(child))
                recurse(modelMatrix, *rEntt);
        }
    };
    recurse(math::mat4x4(1.0f), entt);
}

void Renderer::endScene()
{
    m_api->beginFrame();
    m_api->beginOnScreenRenderPass();

    math::mat4x4 vpMatrix = m_projectionMatrix * m_camera->viewMatrix();

    for (auto& renderables : m_renderables)
    {
        for (auto& renderable : renderables.val) {
            IRenderMethod::Uniforms uniforms = {
                renderable.modelMatrix,
                vpMatrix,
                m_camera->position,
                m_pointLights,
                *renderable.material
            };
            renderable.material->renderMethod->use(uniforms);
            m_api->useVertexBuffer(renderable.vertexBuffer);
            m_api->drawIndexedVertices(renderable.indexBuffer);
        }
    }

    m_imguiCalls();

    m_api->endOnScreenRenderPass();
    m_api->endFrame();

    m_renderables.clear();
    m_pointLights.clear();
}

Renderer::~Renderer()
{
    m_window->clearCallbacks(this);
}