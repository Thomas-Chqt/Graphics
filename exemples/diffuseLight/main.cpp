/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/15 21:19:13
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
#include "Vertex.hpp"
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

struct GrassCube : DrawableEntity
{
    utils::SharedPtr<gfx::VertexBuffer> vertexBuffer;
    utils::SharedPtr<gfx::IndexBuffer> indexBufferSides;
    utils::SharedPtr<gfx::IndexBuffer> indexBufferTop;
    utils::SharedPtr<gfx::IndexBuffer> indexBufferBottom;
    
    utils::SharedPtr<gfx::Texture> sideTexture;
    utils::SharedPtr<gfx::Texture> topTexture;
    utils::SharedPtr<gfx::Texture> bottomTexture;

    GrassCube(const utils::SharedPtr<gfx::GraphicAPI>& api)
    {
        vertexBuffer = api->newVertexBuffer<Vertex>(
        {
            // Front face
            {{ -0.5, -0.5, -0.5 }, { 0.0, 1.0 }, { 0.0, 0.0, -1.0 }},
            {{ -0.5,  0.5, -0.5 }, { 0.0, 0.0 }, { 0.0, 0.0, -1.0 }},
            {{  0.5,  0.5, -0.5 }, { 1.0, 0.0 }, { 0.0, 0.0, -1.0 }},
            {{  0.5, -0.5, -0.5 }, { 1.0, 1.0 }, { 0.0, 0.0, -1.0 }},
            
            // Back face
            {{ -0.5, -0.5,  0.5 }, { 0.0, 1.0 }, { 0.0, 0.0, 1.0 }},
            {{ -0.5,  0.5,  0.5 }, { 0.0, 0.0 }, { 0.0, 0.0, 1.0 }},
            {{  0.5,  0.5,  0.5 }, { 1.0, 0.0 }, { 0.0, 0.0, 1.0 }},
            {{  0.5, -0.5,  0.5 }, { 1.0, 1.0 }, { 0.0, 0.0, 1.0 }},
            
            // Top face
            {{ -0.5,  0.5, -0.5 }, { 0.0, 1.0 }, { 0.0, 1.0, 0.0 }},
            {{ -0.5,  0.5,  0.5 }, { 0.0, 0.0 }, { 0.0, 1.0, 0.0 }},
            {{  0.5,  0.5,  0.5 }, { 1.0, 0.0 }, { 0.0, 1.0, 0.0 }},
            {{  0.5,  0.5, -0.5 }, { 1.0, 1.0 }, { 0.0, 1.0, 0.0 }},
            
            // Bottom face
            {{ -0.5, -0.5, -0.5 }, { 0.0, 1.0 }, { 0.0, -1.0, 0.0 }},
            {{ -0.5, -0.5,  0.5 }, { 0.0, 0.0 }, { 0.0, -1.0, 0.0 }},
            {{  0.5, -0.5,  0.5 }, { 1.0, 0.0 }, { 0.0, -1.0, 0.0 }},
            {{  0.5, -0.5, -0.5 }, { 1.0, 1.0 }, { 0.0, -1.0, 0.0 }},
            
            // Left face
            {{ -0.5, -0.5,  0.5 }, { 0.0, 1.0 }, { -1.0, 0.0, 0.0 }},
            {{ -0.5,  0.5,  0.5 }, { 0.0, 0.0 }, { -1.0, 0.0, 0.0 }},
            {{ -0.5,  0.5, -0.5 }, { 1.0, 0.0 }, { -1.0, 0.0, 0.0 }},
            {{ -0.5, -0.5, -0.5 }, { 1.0, 1.0 }, { -1.0, 0.0, 0.0 }},
            
            // Right face
            {{  0.5, -0.5, -0.5 }, { 0.0, 1.0 }, { 1.0, 0.0, 0.0 }},
            {{  0.5,  0.5, -0.5 }, { 0.0, 0.0 }, { 1.0, 0.0, 0.0 }},
            {{  0.5,  0.5,  0.5 }, { 1.0, 0.0 }, { 1.0, 0.0, 0.0 }},
            {{  0.5, -0.5,  0.5 }, { 1.0, 1.0 }, { 1.0, 0.0, 0.0 }}
        });

        indexBufferSides = api->newIndexBuffer(
        {
             0,  1,  2,  2,  3,  0, // Front face
             4,  5,  6,  6,  7,  4, // Back face
            16, 17, 18, 18, 19, 16, // Left face
            20, 21, 22, 22, 23, 20  // Right face
        });

        indexBufferTop = api->newIndexBuffer({ 8, 9, 10, 10, 11, 8 });
        indexBufferBottom = api->newIndexBuffer({ 12, 13, 14, 14, 15, 12 });

        graphicPipeline = makePipeline(api, "diffuseLight");

        sideTexture = textureFromFile(api, RESSOURCES_DIR"/mc_grass_side.jpg");
        topTexture = textureFromFile(api, RESSOURCES_DIR"/mc_grass_top.jpg");
        bottomTexture = textureFromFile(api, RESSOURCES_DIR"/mc_grass_bottom.jpg");

        position = {0, 0, 3.5};
        rotation = {5.7, 0.8, 0};
    }

    void draw(const utils::SharedPtr<gfx::GraphicAPI>& api) const override
    {
        api->useVertexBuffer(vertexBuffer);
        api->setVertexUniform(graphicPipeline->findVertexUniformIndex("u_modelMatrix"), modelMatrix());
        api->setFragmentTexture(graphicPipeline->findFragmentUniformIndex("u_texture"), sideTexture);
        api->drawIndexedVertices(indexBufferSides);
        api->setFragmentTexture(graphicPipeline->findFragmentUniformIndex("u_texture"), topTexture);
        api->drawIndexedVertices(indexBufferTop);
        api->setFragmentTexture(graphicPipeline->findFragmentUniformIndex("u_texture"), bottomTexture);
        api->drawIndexedVertices(indexBufferBottom);
    }
};

struct Camera : public Entity
{
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
    GrassCube cube(graphicAPI);
    Camera camera;
    Light light;

    window->setEventCallBack([&](gfx::Event& event)
    {
        event.dispatch<gfx::KeyDownEvent>([&](gfx::KeyDownEvent& event)
        {
            switch (event.keyCode())
            {
            case ESC_KEY:
                running = false;
                break;
            case W_KEY:
                camera.position += camera.forward() * 0.1;
                break;
            case A_KEY:
                camera.position -= camera.right() * 0.1;
                break;
            case S_KEY:
                camera.position -= camera.forward() * 0.1;
                break;
            case D_KEY:
                camera.position += camera.right() * 0.1;
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

        ImGui::Text("Cube Transform :");
        ImGui::SliderFloat("Pos X", &cube.position.x, -5, 5);
        ImGui::SliderFloat("Pos Y", &cube.position.y, -5, 5);
        ImGui::SliderFloat("Pos Z", &cube.position.z, -1, 10);
        ImGui::SliderFloat("Rot X", &cube.rotation.x, 0, 2 * PI);
        ImGui::SliderFloat("Rot Y", &cube.rotation.y, 0, 2 * PI);
        ImGui::SliderFloat("Rot Z", &cube.rotation.z, 0, 2 * PI);

        ImGui::Spacing();

        ImGui::Text("Light properties :");
        ImGui::ColorPicker4("color", (float*)&light.color);
        ImGui::SliderFloat("Intensity", &light.intensity, 0, 1);
        ImGui::SliderFloat("Light Rot X", &light.rotation.x, 0, 2 * PI);
        ImGui::SliderFloat("Light Rot Y", &light.rotation.y, 0, 2 * PI);

        render(
            graphicAPI,
            cube,
            makeProjectionMatrix(window),
            camera,
            light
        );

        graphicAPI->endOnScreenRenderPass();
        graphicAPI->endFrame();
    }

    gfx::Platform::terminate();
}