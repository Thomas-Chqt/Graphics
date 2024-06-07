/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/07 16:38:07
 * ---------------------------------------------------
 */

#include "Graphics/Event.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/ShaderLibrary.hpp"

#include "Graphics/VertexBuffer.hpp"
#include "Graphics/Window.hpp"
#include "Math/Vector.hpp"
#include "Math/Constants.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"

#include "Vertex.hpp"

int main()
{
    gfx::Platform::init();
    gfx::ShaderLibrary::init();

    #ifdef GFX_METAL_ENABLED
        gfx::ShaderLibrary::shared().setMetalShaderLibPath(MTL_SHADER_LIB);
    #endif

    gfx::ShaderLibrary::shared().registerShader("flatColorCube_vs"
        #if GFX_METAL_ENABLED
            , "flatColorCube_vs"
        #endif
        #if GFX_OPENGL_ENABLED
            , utils::String::contentOfFile(OPENGL_SHADER_DIR"/flatColorCube.vs")
        #endif
    );
    gfx::ShaderLibrary::shared().registerShader("flatColorCube_fs"
        #if GFX_METAL_ENABLED
            , "flatColorCube_fs"
        #endif
        #if GFX_OPENGL_ENABLED
            , utils::String::contentOfFile(OPENGL_SHADER_DIR"/flatColorCube.fs")
        #endif
    );

    const utils::Array<Vertex> vertices = {
        { -0.5, -0.5, -0.5 },
        { -0.5,  0.5, -0.5 },
        {  0.5,  0.5, -0.5 },
        {  0.5, -0.5, -0.5 },
        {  0.5, -0.5,  0.5 },
        {  0.5,  0.5,  0.5 },
        { -0.5,  0.5,  0.5 },
        { -0.5, -0.5,  0.5 }
    };

    const utils::Array<utils::uint32> indices = { 
        0, 1, 2, 0, 2, 3, // Front
        4, 5, 6, 4, 6, 7, // Back
        3, 2, 5, 3, 5, 4, // Right
        7, 6, 1, 7, 1, 0, // Left
        1, 6, 5, 1, 5, 2, // Top
        0, 3, 4, 0, 4, 7  // Bottom
    };


    utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newDefaultWindow(800, 600);
    utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newDefaultGraphicAPI(window);

    graphicAPI->useForImGui(
        ImGuiConfigFlags_NavEnableKeyboard |
        ImGuiConfigFlags_DockingEnable     |
        ImGuiConfigFlags_ViewportsEnable
    );

    utils::SharedPtr<gfx::GraphicPipeline> graphicPipeline = graphicAPI->newGraphicsPipeline("flatColorCube_vs", "flatColorCube_fs");
    utils::SharedPtr<gfx::VertexBuffer> vertexBuffer = graphicAPI->newVertexBuffer(vertices);
    utils::SharedPtr<gfx::IndexBuffer> indexBuffer = graphicAPI->newIndexBuffer(indices);

    bool running = true;

    window->setEventCallBack([&](gfx::Event& event)
    {
        event.dispatch<gfx::KeyDownEvent>([&](gfx::KeyDownEvent& event)
        {
            switch (event.keyCode())
            {
            case ESC_KEY:
                running = false;
            }
        });
    });

    float fov = 60 * (PI / 180.0f);
    float aspectRatio = 800.0f / 600.0f;
    float zNear = 0.1f;
    float zFar = 100;

    float ys = 1 / std::tanf(fov * 0.5);
    float xs = ys / aspectRatio;
    float zs = zFar / (zFar - zNear);

    math::mat4x4 projectionMatrix (xs,   0,  0,           0,
                                    0,  ys,  0,           0,
                                    0,   0, zs, -zNear * zs,
                                    0,   0,  1,           0);

    math::vec3f cubePos(0, 0, 3);
    math::rgba cubeColor = WHITE;

    while (running)
    {
        gfx::Platform::shared().pollEvents();
        
        math::mat4x4 modelMatrix(1, 0, 0, cubePos.x,
                                 0, 1, 0, cubePos.y,
                                 0, 0, 1, cubePos.z,
                                 0, 0, 0, 1);

        graphicAPI->beginFrame();

        ImGui::Text("FPS : %.1f", ImGui::GetIO().Framerate);
        ImGui::SliderFloat("Pos X", &cubePos.x, -5, 5);
        ImGui::SliderFloat("Pos Y", &cubePos.y, -5, 5);
        ImGui::SliderFloat("Pos Z", &cubePos.z, -1, 10);
        ImGui::ColorPicker4("u_color", (float *)&cubeColor);

        graphicAPI->useGraphicsPipeline(graphicPipeline);
        graphicAPI->useVertexBuffer(vertexBuffer);
        graphicAPI->setFragmentUniform(graphicPipeline->findFragmentUniformIndex("u_color"), cubeColor);
        graphicAPI->setVertexUniform(graphicPipeline->findVertexUniformIndex("u_MVPMatrix"), projectionMatrix * modelMatrix);
        graphicAPI->drawIndexedVertices(indexBuffer);

        graphicAPI->endFrame();
    }

    gfx::ShaderLibrary::terminated();
    gfx::Platform::terminate();
}