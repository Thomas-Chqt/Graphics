/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/19 15:53:34
 * ---------------------------------------------------
 */

#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/Window.hpp"
#include "Math/Constants.hpp"
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"
#include "helpers.hpp"
#include "imgui/imgui.h"

struct Entity
{
    math::vec3f position;
    math::vec3f rotation;

    inline math::mat4x4 modelMatrix() const { return math::mat4x4::translation(position) * math::mat4x4::rotation(rotation); }
    inline math::vec3f forward() const { return (math::mat4x4::rotation({0, rotation.y, 0}) * math::mat4x4::rotation({rotation.x, 0, 0}) * math::vec4f({0, 0, 1, 1})).xyz(); }
    inline math::vec3f right() const   { return (math::mat4x4::rotation({0, rotation.y, 0}) * math::mat4x4::rotation({rotation.x, 0, 0}) * math::vec4f({1, 0, 0, 1})).xyz(); }

};

struct DrawableEntity : Entity
{
    utils::SharedPtr<gfx::GraphicPipeline> graphicPipeline;
    virtual void draw(const utils::SharedPtr<gfx::GraphicAPI>& api) const = 0;
};

struct Teapot : DrawableEntity
{
    utils::Array<SubMesh> subMeshes;

    Teapot(const utils::SharedPtr<gfx::GraphicAPI>& api)
    {
        graphicPipeline = makePipeline(api, "phongLight");
        subMeshes = loadModel(api, RESSOURCES_DIR"/teapot.obj");

        position = {0, 0, 0};
        rotation = {0, 0, 0};
    }

    void draw(const utils::SharedPtr<gfx::GraphicAPI>& api) const override
    {
        for (auto& mesh : subMeshes)
        {
            api->useVertexBuffer(mesh.vertexBuffer);
            api->setVertexUniform(graphicPipeline->findVertexUniformIndex("u_modelMatrix"), modelMatrix());
            api->drawIndexedVertices(mesh.indexBuffer);
        }
    }
};

struct Camera : public Entity
{
    Camera()
    {
        position = {0, 150, -150};
        rotation = {PI/5, 0, 0};
    }

    math::mat4x4 viewMatrix() const
    {
        math::vec3f normF = forward().normalized();
        math::vec3f normR = right().normalized();
        math::vec3f normU = cross(normF, normR);

        return math::mat4x4(normR.x, normR.y, normR.z, -dot(normR, position),
                            normU.x, normU.y, normU.z, -dot(normU, position),
                            normF.x, normF.y, normF.z, -dot(normF, position),
                                  0,       0,       0,                    1);
    }
};

struct Light : public Entity
{
    math::rgba color = WHITE;
    float intensity = 1;
};

void render(
    const utils::SharedPtr<gfx::GraphicAPI>& api,
    const DrawableEntity& entt,
    const math::mat4x4& proj,
    const Camera& camera,
    const Light& light
)
{
    api->useGraphicsPipeline(entt.graphicPipeline);
    api->setVertexUniform(entt.graphicPipeline->findVertexUniformIndex("u_vpMatrix"), proj * camera.viewMatrix());
    api->setFragmentUniform(entt.graphicPipeline->findFragmentUniformIndex("u_diffuseColor"), light.color);
    api->setFragmentUniform(entt.graphicPipeline->findFragmentUniformIndex("u_diffuseIntensity"), light.intensity);
    api->setFragmentUniform(entt.graphicPipeline->findFragmentUniformIndex("u_diffuseDirection"), light.forward().normalized());
    entt.draw(api);
}

int main()
{
    gfx::Platform::init();

    utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newDefaultWindow(800, 600);
    utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newDefaultGraphicAPI(window);

    #ifdef GFX_METAL_ENABLED
        graphicAPI->initMetalShaderLib(MTL_SHADER_LIB);
    #endif    
    graphicAPI->useForImGui(ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable);


    bool running = true;
    Teapot teapot(graphicAPI);
    Camera camera;
    Light light;

    gfx::Platform::shared().addEventCallBack([&](gfx::Event& event)
    {
        event.dispatch<gfx::KeyDownEvent>([&](gfx::KeyDownEvent& event)
        {
            switch (event.keyCode())
            {
            case ESC_KEY:
                running = false;
                break;
            case W_KEY:
                camera.position += camera.forward() * 2;
                break;
            case A_KEY:
                camera.position -= camera.right() * 2;
                break;
            case S_KEY:
                camera.position -= camera.forward() * 2;
                break;
            case D_KEY:
                camera.position += camera.right() * 2;
                break;
            case UP_KEY:
                camera.rotation.x -= 0.1;
                break;
            case LEFT_KEY:
                camera.rotation.y -= 0.1;
                break;
            case DOWN_KEY:
                camera.rotation.x += 0.1;
                break;
            case RIGHT_KEY:
                camera.rotation.y += 0.1;
                break;
            }
        });
    });

    while (running)
    {
        gfx::Platform::shared().pollEvents();

        graphicAPI->beginFrame();
        graphicAPI->beginOnScreenRenderPass();

        ImGui::Text("FPS : %f", ImGui::GetIO().Framerate);

        ImGui::Text("Teapot Transform :");
        ImGui::SliderFloat("Pos X", &teapot.position.x, -100, 100);
        ImGui::SliderFloat("Pos Y", &teapot.position.y, -100, 100);
        ImGui::SliderFloat("Pos Z", &teapot.position.z, -100, 100);
        ImGui::SliderFloat("Rot X", &teapot.rotation.x, 0, 2 * PI);
        ImGui::SliderFloat("Rot Y", &teapot.rotation.y, 0, 2 * PI);
        ImGui::SliderFloat("Rot Z", &teapot.rotation.z, 0, 2 * PI);

        ImGui::Spacing();

        ImGui::Text("Light properties :");
        ImGui::ColorPicker4("color", (float*)&light.color);
        ImGui::SliderFloat("Intensity", &light.intensity, 0, 1);
        ImGui::SliderFloat("Light Rot X", &light.rotation.x, 0, 2 * PI);
        ImGui::SliderFloat("Light Rot Y", &light.rotation.y, 0, 2 * PI);

        render(
            graphicAPI,
            teapot,
            makeProjectionMatrix(window),
            camera,
            light
        );

        graphicAPI->endOnScreenRenderPass();
        graphicAPI->endFrame();
    }

    gfx::Platform::terminate();
}
