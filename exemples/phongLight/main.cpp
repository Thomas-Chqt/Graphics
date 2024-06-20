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

struct Cup : DrawableEntity
{
    utils::Array<SubMesh> subMeshes;

    Cup(const utils::SharedPtr<gfx::GraphicAPI>& api)
    {
        graphicPipeline = makePipeline(api, "phongLight");
        subMeshes = loadModel(api, RESSOURCES_DIR"/cup.obj");

        position = {0, -1.5, 5};
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
        position = {0, 0, 0};
        rotation = {0, 0, 0};
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
    float ambiantIntensity  = 0.5;
    float diffuseIntensity  = 1;
    float specularIntensity = 1;
    float specularPower     = 1;
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

    api->setFragmentUniform(entt.graphicPipeline->findFragmentUniformIndex("u_lightColor"),        light.color.xyz());
    api->setFragmentUniform(entt.graphicPipeline->findFragmentUniformIndex("u_lightDirection"),    light.forward().normalized());
    api->setFragmentUniform(entt.graphicPipeline->findFragmentUniformIndex("u_ambiantIntensity"),  light.ambiantIntensity);
    api->setFragmentUniform(entt.graphicPipeline->findFragmentUniformIndex("u_diffuseIntensity"),  light.diffuseIntensity);
    api->setFragmentUniform(entt.graphicPipeline->findFragmentUniformIndex("u_specularIntensity"), light.specularIntensity);
    api->setFragmentUniform(entt.graphicPipeline->findFragmentUniformIndex("u_specularPower"),     light.specularPower);
    api->setFragmentUniform(entt.graphicPipeline->findFragmentUniformIndex("u_cameraPos"),         camera.position);

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
    Cup cup(graphicAPI);
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
            }
        });
    });

    while (running)
    {
        gfx::Platform::shared().pollEvents();

        if (window->isKeyPress(W_KEY))
            camera.position += camera.forward() * 0.2;
        if (window->isKeyPress(A_KEY))
            camera.position -= camera.right() * 0.2;
        if (window->isKeyPress(S_KEY))
            camera.position -= camera.forward() * 0.2;
        if (window->isKeyPress(D_KEY))
            camera.position += camera.right() * 0.2;
        if (window->isKeyPress(UP_KEY))
            camera.rotation.x -= 0.05;
        if (window->isKeyPress(LEFT_KEY))
            camera.rotation.y -= 0.05;
        if (window->isKeyPress(DOWN_KEY))
            camera.rotation.x += 0.05;
        if (window->isKeyPress(RIGHT_KEY))
            camera.rotation.y += 0.05;

        graphicAPI->beginFrame();
        graphicAPI->beginOnScreenRenderPass();

        ImGui::Text("FPS : %f", ImGui::GetIO().Framerate);

        ImGui::Text("Cup Transform :");
        ImGui::SliderFloat("Pos X", &cup.position.x, -5, 5);
        ImGui::SliderFloat("Pos Y", &cup.position.y, -5, 5);
        ImGui::SliderFloat("Pos Z", &cup.position.z, -5, 5);
        ImGui::SliderFloat("Rot X", &cup.rotation.x, 0, 2 * PI);
        ImGui::SliderFloat("Rot Y", &cup.rotation.y, 0, 2 * PI);
        ImGui::SliderFloat("Rot Z", &cup.rotation.z, 0, 2 * PI);

        ImGui::Spacing();

        ImGui::Text("Light properties :");
        ImGui::ColorPicker4("color", (float*)&light.color);
        ImGui::SliderFloat("Light Rot X", &light.rotation.x, 0, 2 * PI);
        ImGui::SliderFloat("Light Rot Y", &light.rotation.y, 0, 2 * PI);
        ImGui::SliderFloat("ambiantIntensity",  &light.ambiantIntensity,  0, 2);
        ImGui::SliderFloat("diffuseIntensity",  &light.diffuseIntensity,  0, 2);
        ImGui::SliderFloat("specularIntensity", &light.specularIntensity, 0, 2);
        ImGui::SliderFloat("specularPower",     &light.specularPower,     0, 100);

        render(
            graphicAPI,
            cup,
            makeProjectionMatrix(window),
            camera,
            light
        );

        graphicAPI->endOnScreenRenderPass();
        graphicAPI->endFrame();
    }

    gfx::Platform::terminate();
}
