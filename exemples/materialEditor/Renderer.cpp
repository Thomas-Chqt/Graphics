/*
 * ---------------------------------------------------
 * Renderer.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/06 19:22:45
 * ---------------------------------------------------
 */

#include "Renderer.hpp"
#include "DirectionalLight.hpp"
#include "Graphics/Buffer.hpp"
#include "Material.hpp"
#include "Math/Constants.hpp"
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
#include "Mesh.hpp"
#include "RenderMethod.hpp"
#include "UtilsCPP/Func.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/Types.hpp"
#include <cmath>

Renderer::Renderer(const utils::SharedPtr<gfx::GraphicAPI>& api, const utils::SharedPtr<gfx::Window>& window)
    : m_window(window), m_graphicAPI(api),
      m_phongRenderMethod(api), m_skyBoxRenderMethod(api)
{
    utils::Func<void(gfx::Event&)> updateVPMatrix = [this](gfx::Event& event) {
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
    updateVPMatrix(ev);
    m_window->addEventCallBack(updateVPMatrix, this);

    m_viewMatrix = math::mat4x4::translation({0, 0, -3}).inversed();

    SkyboxRenderMethod::Vertex skyboxVertices[] = {
        { { -1.0F,  1.0F, -1.0F } },
        { { -1.0F, -1.0F, -1.0F } },
        { {  1.0F, -1.0F, -1.0F } },
        { {  1.0F, -1.0F, -1.0F } },
        { {  1.0F,  1.0F, -1.0F } },
        { { -1.0F,  1.0F, -1.0F } },
        { { -1.0F, -1.0F,  1.0F } },
        { { -1.0F, -1.0F, -1.0F } },
        { { -1.0F,  1.0F, -1.0F } },
        { { -1.0F,  1.0F, -1.0F } },
        { { -1.0F,  1.0F,  1.0F } },
        { { -1.0F, -1.0F,  1.0F } },
        { {  1.0F, -1.0F, -1.0F } },
        { {  1.0F, -1.0F,  1.0F } },
        { {  1.0F,  1.0F,  1.0F } },
        { {  1.0F,  1.0F,  1.0F } },
        { {  1.0F,  1.0F, -1.0F } },
        { {  1.0F, -1.0F, -1.0F } },
        { { -1.0F, -1.0F,  1.0F } },
        { { -1.0F,  1.0F,  1.0F } },
        { {  1.0F,  1.0F,  1.0F } },
        { {  1.0F,  1.0F,  1.0F } },
        { {  1.0F, -1.0F,  1.0F } },
        { { -1.0F, -1.0F,  1.0F } },
        { { -1.0F,  1.0F, -1.0F } },
        { {  1.0F,  1.0F, -1.0F } },
        { {  1.0F,  1.0F,  1.0F } },
        { {  1.0F,  1.0F,  1.0F } },
        { { -1.0F,  1.0F,  1.0F } },
        { { -1.0F,  1.0F, -1.0F } },
        { { -1.0F, -1.0F, -1.0F } },
        { { -1.0F, -1.0F,  1.0F } },
        { {  1.0F, -1.0F, -1.0F } },
        { {  1.0F, -1.0F, -1.0F } },
        { { -1.0F, -1.0F,  1.0F } },
        { {  1.0F, -1.0F,  1.0F } }
    };

    gfx::Buffer::Descriptor bufferDescriptor;
    bufferDescriptor.debugName = "skybox vertex buffer";
    bufferDescriptor.size = sizeof(skyboxVertices);
    bufferDescriptor.initialData = skyboxVertices;

    m_skyBoxVertexBuffer = m_graphicAPI->newBuffer(bufferDescriptor);
}

void Renderer::render(const Mesh& mesh, const Material& material, const DirectionalLight& light, const math::mat4x4& transform)
{
    m_phongRenderMethod.use();

    m_phongRenderMethod.setViewMatrix(m_viewMatrix);
    m_phongRenderMethod.setProjectionMatrix(m_projectionMatrix);
    m_phongRenderMethod.setMaterial(material);
    m_phongRenderMethod.setLight(light);

    utils::Func<void(const SubMesh&, const math::mat4x4&)> renderSubMesh = [&](const SubMesh& subMesh, const math::mat4x4& transform) {
        math::mat4x4 modelMatrix = transform * subMesh.transform;
        if (subMesh.vertexBuffer && subMesh.indexBuffer)
        {
            m_phongRenderMethod.setModelMatrix(modelMatrix);
            m_graphicAPI->setVertexBuffer(subMesh.vertexBuffer, 0);
            m_graphicAPI->drawIndexedVertices(subMesh.indexBuffer);
        }
        for (const auto& subMesh : subMesh.childs)
            renderSubMesh(subMesh, modelMatrix);
    };
    for (const auto& subMesh : mesh.subMeshes)
        renderSubMesh(subMesh, transform);
}

void Renderer::renderSkybox(const utils::SharedPtr<gfx::Texture>& texture)
{
    m_skyBoxRenderMethod.use();
    m_skyBoxRenderMethod.setViewMatrix(m_viewMatrix);
    m_skyBoxRenderMethod.setProjectionMatrix(m_projectionMatrix);
    m_skyBoxRenderMethod.setTextureCube(texture);
    m_graphicAPI->setVertexBuffer(m_skyBoxVertexBuffer, 0);
    m_graphicAPI->drawVertices(0, 36);
}

Renderer::~Renderer()
{
    m_window->clearCallbacks(this);
}

