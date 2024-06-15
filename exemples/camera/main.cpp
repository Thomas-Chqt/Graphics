/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/14 23:37:10
 * ---------------------------------------------------
 */

#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/Window.hpp"
#include "Math/Constants.hpp"
#include "Math/Matrix.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"
#include "Vertex.hpp"
#include "helpers.hpp"

struct Entity
{
    math::vec3f position;
    math::vec3f rotation;

    inline math::mat4x4 modelMatrix() const { return math::mat4x4::translation(position) * math::mat4x4::rotation(rotation); }
    inline math::vec3f forward() const { return (math::mat4x4::rotation({0, rotation.y, 0}) * math::mat4x4::rotation({rotation.x, 0, 0}) * math::vec4f({0, 0, 1, 1})).xyz(); }
    inline math::vec3f right() const   { return (math::mat4x4::rotation({0, rotation.y, 0}) * math::mat4x4::rotation({rotation.x, 0, 0}) * math::vec4f({1, 0, 0, 1})).xyz(); }

};

struct GrassCube : Entity
{
    utils::SharedPtr<gfx::VertexBuffer> vertexBuffer;
    utils::SharedPtr<gfx::IndexBuffer> indexBufferSides;
    utils::SharedPtr<gfx::IndexBuffer> indexBufferTop;
    utils::SharedPtr<gfx::IndexBuffer> indexBufferBottom;
    
    utils::SharedPtr<gfx::GraphicPipeline> graphicPipeline;
    utils::SharedPtr<gfx::Texture> sideTexture;
    utils::SharedPtr<gfx::Texture> topTexture;
    utils::SharedPtr<gfx::Texture> bottomTexture;

    GrassCube(const utils::SharedPtr<gfx::GraphicAPI>& api)
    {
        vertexBuffer = api->newVertexBuffer<Vertex>(
        {
            {{ -0.5, -0.5, -0.5 }, { 0.0, 1.0 }},
            {{ -0.5,  0.5, -0.5 }, { 0.0, 0.0 }},
            {{  0.5,  0.5, -0.5 }, { 1.0, 0.0 }},
            {{  0.5, -0.5, -0.5 }, { 1.0, 1.0 }},
            {{ -0.5, -0.5,  0.5 }, { 0.0, 1.0 }},
            {{ -0.5,  0.5,  0.5 }, { 0.0, 0.0 }},
            {{  0.5,  0.5,  0.5 }, { 1.0, 0.0 }},
            {{  0.5, -0.5,  0.5 }, { 1.0, 1.0 }},
            {{ -0.5,  0.5, -0.5 }, { 0.0, 1.0 }},
            {{ -0.5,  0.5,  0.5 }, { 0.0, 0.0 }},
            {{  0.5,  0.5,  0.5 }, { 1.0, 0.0 }},
            {{  0.5,  0.5, -0.5 }, { 1.0, 1.0 }},
            {{ -0.5, -0.5, -0.5 }, { 0.0, 1.0 }},
            {{ -0.5, -0.5,  0.5 }, { 0.0, 0.0 }},
            {{  0.5, -0.5,  0.5 }, { 1.0, 0.0 }},
            {{  0.5, -0.5, -0.5 }, { 1.0, 1.0 }},
            {{ -0.5, -0.5,  0.5 }, { 0.0, 1.0 }},
            {{ -0.5,  0.5,  0.5 }, { 0.0, 0.0 }},
            {{ -0.5,  0.5, -0.5 }, { 1.0, 0.0 }},
            {{ -0.5, -0.5, -0.5 }, { 1.0, 1.0 }},
            {{  0.5, -0.5, -0.5 }, { 0.0, 1.0 }},
            {{  0.5,  0.5, -0.5 }, { 0.0, 0.0 }},
            {{  0.5,  0.5,  0.5 }, { 1.0, 0.0 }},
            {{  0.5, -0.5,  0.5 }, { 1.0, 1.0 }}
        });

        indexBufferSides = api->newIndexBuffer(
        {
            0, 1, 2, 2, 3, 0, // Front face
            4, 5, 6, 6, 7, 4, // Back face
            8, 9, 10, 10, 11, 8, // Top face
            12, 13, 14, 14, 15, 12, // Bottom face
            16, 17, 18, 18, 19, 16, // Left face
            20, 21, 22, 22, 23, 20 // Right face
        });

        indexBufferTop = api->newIndexBuffer(
        {
            8, 9, 10, 10, 11, 8 // Top face
        });

        indexBufferBottom = api->newIndexBuffer(
        {
            12, 13, 14, 14, 15, 12 // Bottom face
        });

        graphicPipeline = makePipeline(api, "texturedCube2");

        sideTexture = textureFromFile(api, RESSOURCES_DIR"/mc_grass_side.jpg");
        topTexture = textureFromFile(api, RESSOURCES_DIR"/mc_grass_top.jpg");
        bottomTexture = textureFromFile(api, RESSOURCES_DIR"/mc_grass_bottom.jpg");

        position = {0, 0, 3};
        rotation = {0, 0, 0};
    }

    void render(const utils::SharedPtr<gfx::GraphicAPI>& api, math::mat4x4 vpMatrix)
    {
        api->useGraphicsPipeline(graphicPipeline);
        api->useVertexBuffer(vertexBuffer);

        api->setVertexUniform(graphicPipeline->findVertexUniformIndex("u_modelMatrix"), modelMatrix());
        api->setVertexUniform(graphicPipeline->findVertexUniformIndex("u_vpMatrix"), vpMatrix);
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
    math::mat4x4 viewMatrix()
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

int main()
{
    gfx::Platform::init();

    utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newDefaultWindow(800, 600);
    utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newDefaultGraphicAPI(window);

    #ifdef GFX_METAL_ENABLED
        graphicAPI->initMetalShaderLib(MTL_SHADER_LIB);
    #endif    
    graphicAPI->useForImGui(
        ImGuiConfigFlags_NavEnableKeyboard |
        ImGuiConfigFlags_DockingEnable     |
        ImGuiConfigFlags_ViewportsEnable
    );

    float fov = 60 * (PI / 180.0f);
    float aspectRatio = 800.0f / 600.0f;
    float zNear = 0.1f;
    float zFar = 100;

    float ys = 1 / std::tan(fov * 0.5);
    float xs = ys / aspectRatio;
    float zs = zFar / (zFar - zNear);

    math::mat4x4 projectionMatrix = math::mat4x4 (xs,   0,  0,           0,
                                                   0,  ys,  0,           0,
                                                   0,   0, zs, -zNear * zs,
                                                   0,   0,  1,           0);

    GrassCube cube(graphicAPI);
    Camera camera;

    bool running = true;
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

        ImGui::SliderFloat("Pos X", &cube.position.x, -5, 5);
        ImGui::SliderFloat("Pos Y", &cube.position.y, -5, 5);
        ImGui::SliderFloat("Pos Z", &cube.position.z, -1, 10);

        ImGui::SliderFloat("Rot X", &cube.rotation.x, 0, 2 * PI);
        ImGui::SliderFloat("Rot Y", &cube.rotation.y, 0, 2 * PI);
        ImGui::SliderFloat("Rot Z", &cube.rotation.z, 0, 2 * PI);

        cube.render(graphicAPI, projectionMatrix * camera.viewMatrix());

        graphicAPI->endOnScreenRenderPass();
        graphicAPI->endFrame();
    }

    gfx::Platform::terminate();
}