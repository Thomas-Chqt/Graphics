/*
 * ---------------------------------------------------
 * Renderer.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/17 16:51:24
 * ---------------------------------------------------
 */

#include "Renderer.hpp"
#include "EntityComponent.hpp"
#include "Graphics/BufferInstance.hpp"
#include "Math/Constants.hpp"
#include "Math/Matrix.hpp"
#include "RenderMethod.hpp"
#include <cmath>

Renderer::Renderer(const utils::SharedPtr<gfx::Window>& window, const utils::SharedPtr<gfx::GraphicAPI>& api)
    : m_window(window), m_api(api),
      m_matrixBuffer(gfx::BufferInstance<RenderMethod::MatrixBuffer>::alloc(m_api)),
      m_lightBuffer(gfx::BufferInstance<RenderMethod::LightBuffer>::alloc(m_api)),
      m_cameraBuffer(gfx::BufferInstance<RenderMethod::CameraBuffer>::alloc(m_api))
{
    utils::Func<void(gfx::Event&)> updateProjectionMatrix = [this](gfx::Event& event) {
        event.dispatch<gfx::WindowResizeEvent>([this](gfx::WindowResizeEvent& event) {
            float fov = 60 * (PI / 180.0F);
            float aspectRatio = (float)event.width() / (float)event.height();
            float zNear = 0.1F;
            float zFar = 10000;

            float ys = 1.0F / std::tan(fov * 0.5F);
            float xs = ys / aspectRatio;
            float zs = zFar / (zFar - zNear);

            m_projectionMatrix = math::mat4x4(xs,   0,  0,           0,
                                               0,  ys,  0,           0,
                                               0,   0, zs, -zNear * zs,
                                               0,   0,  1,           0);
        });
    };

    utils::uint32 w = 0;
    utils::uint32 h = 0;
    m_window->getWindowSize(&w, &h);
    gfx::WindowResizeEvent ev(*m_window, (int)w, (int)h);
    updateProjectionMatrix(ev);
    m_window->addEventCallBack(updateProjectionMatrix, this);
}

void Renderer::beginScene()
{
    m_matrixBuffer.map();
    m_lightBuffer.map();
    m_cameraBuffer.map();

    m_lightBuffer.content().directionalLightCount = 0;
    m_lightBuffer.content().pointLightCount = 0;
    m_transformedSubmeshes.clear();
}

void Renderer::addEntity(const Entity& entity)
{
    if (entity.renderableComponent && entity.transformComponent)
    {
        utils::Func<void(const SubMesh&, const math::mat4x4&)> addSubMesh = [&](const SubMesh& subMesh, const math::mat4x4& parentTransform) {
            SubMesh worldSpaceSubMesh = subMesh;
            worldSpaceSubMesh.transform = parentTransform * subMesh.transform;
            if (subMesh.vertexBuffer)
                m_transformedSubmeshes.append(worldSpaceSubMesh);
            for (const auto& child : subMesh.childs)
                addSubMesh(child, worldSpaceSubMesh.transform);
        };
        for (const auto& subMesh : entity.renderableComponent->mesh.subMeshes)
            addSubMesh(subMesh, math::mat4x4::translation(entity.transformComponent->pos) * math::mat4x4::scale(entity.transformComponent->sca) * math::mat4x4::rotation(entity.transformComponent->rot));
    }

    if (entity.lightSourceComponent && entity.transformComponent)
    {
        if (entity.lightSourceComponent->type == LightSourceComponent::Type::point)
        {
            auto& dst = m_lightBuffer.content().pointLights[m_lightBuffer.content().pointLightCount];

            dst.color             = entity.lightSourceComponent->color;
            dst.ambiantIntensity  = entity.lightSourceComponent->ambiantIntensity;
            dst.diffuseIntensity  = entity.lightSourceComponent->diffuseIntensity;
            dst.specularIntensity = entity.lightSourceComponent->specularIntensity;
            dst.position          = entity.transformComponent->pos;

            m_lightBuffer.content().pointLightCount += 1;
        }
    }

    if (entity.viewPointComponent && entity.transformComponent)
    {
        if (entity.viewPointComponent->isActive)
            m_matrixBuffer.content().vpMatrix = m_projectionMatrix * (math::mat4x4::translation(entity.transformComponent->pos) * math::mat4x4::rotation(entity.transformComponent->rot)).inversed();
    }
}

void Renderer::endScene()
{
    m_cameraBuffer.unmap();
    m_lightBuffer.unmap();
    m_matrixBuffer.unmap();


    m_api->beginFrame();
    {
        m_api->beginRenderPass();
        {
            for (const auto& mesh : m_transformedSubmeshes)
            {
                mesh.material->renderMethod->use();
                mesh.material->renderMethod->setMatrixBuffer(m_matrixBuffer.buffer());
                mesh.material->renderMethod->setLightBuffer(m_lightBuffer.buffer());
                mesh.material->renderMethod->setCameraBuffer(m_cameraBuffer.buffer());
                mesh.material->renderMethod->setMaterial(*mesh.material);

                m_matrixBuffer.map();
                m_matrixBuffer.content().modelMatrix = mesh.transform;
                m_matrixBuffer.unmap();

                m_api->setVertexBuffer(mesh.vertexBuffer, 0);
                m_api->drawIndexedVertices(mesh.indexBuffer);
            }
        }
        m_api->endRenderPass();
    }
    m_api->endFrame();
}

Renderer::~Renderer()
{
    m_window->clearCallbacks(this);
}