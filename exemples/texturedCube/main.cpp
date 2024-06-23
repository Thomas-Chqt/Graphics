/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/14 12:30:22
 * ---------------------------------------------------
 */
#include "Graphics/Enums.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "Graphics/Window.hpp"
#include "Math/Constants.hpp"
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"
#include "Vertex.hpp"
#include <cstddef>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <cmath>
#include <map>

template<>
gfx::StructLayout gfx::getLayout<Vertex>()
{
    return {
        { 3, Type::FLOAT, (void*)0 },
        { 2, Type::FLOAT, (void*)offsetof(Vertex, uv) },
    };
}

gfx::GraphicPipeline::Descriptor makeGfxPipelineDescriptor(const utils::String& shaderName)
{
    gfx::GraphicPipeline::Descriptor graphicPipelineDescriptor;
    #ifdef GFX_METAL_ENABLED
        graphicPipelineDescriptor.metalVSFunction = shaderName + "_vs";
        graphicPipelineDescriptor.metalFSFunction = shaderName + "_fs";
    #endif 
    #if GFX_OPENGL_ENABLED
        graphicPipelineDescriptor.openglVSCode = utils::String::contentOfFile(utils::String(OPENGL_SHADER_DIR) + utils::String("/") + shaderName + utils::String(".vs"));
        graphicPipelineDescriptor.openglFSCode = utils::String::contentOfFile(utils::String(OPENGL_SHADER_DIR) + utils::String("/") + shaderName + utils::String(".fs"));
    #endif

    return graphicPipelineDescriptor;
}

utils::SharedPtr<gfx::Texture> textureFromFile(const utils::SharedPtr<gfx::GraphicAPI>& api, const utils::String& path)
{
    int width;
    int height;
    stbi_uc* imgBytes = stbi_load(path, &width, &height, nullptr, STBI_rgb_alpha);

    gfx::Texture::Descriptor textureDescriptor;
    textureDescriptor.width = width;
    textureDescriptor.height = height;

    utils::SharedPtr<gfx::Texture> texture = api->newTexture(textureDescriptor);
    texture->setBytes(imgBytes);

    stbi_image_free(imgBytes);

    return texture;
}

struct Material
{
    utils::SharedPtr<gfx::GraphicPipeline> graphicPipeline;
    std::map<utils::String, utils::SharedPtr<gfx::Texture>> textures;
};

struct Object
{
    utils::SharedPtr<gfx::VertexBuffer> vertexBuffer;
    utils::SharedPtr<gfx::IndexBuffer> indexBuffer;
    Material material;
    math::vec3f position = {0, 0, 3};
    math::vec3f rotation = {0, 0, 0};

    math::mat4x4 modelMatrix() const
    {
        return math::mat4x4(
            1, 0, 0, position.x,
            0, 1, 0, position.y,
            0, 0, 1, position.z,
            0, 0, 0,          1
        ) 
        * math::mat4x4(
            1,                    0,                     0, 0,
            0, std::cos(rotation.x), -std::sin(rotation.x), 0,
            0, std::sin(rotation.x),  std::cos(rotation.x), 0,
            0,                    0,                     0, 1
        ) 
        * math::mat4x4(
             std::cos(rotation.y), 0, std::sin(rotation.y), 0,
                                0, 1,                    0, 0,
            -std::sin(rotation.y), 0, std::cos(rotation.y), 0,
                                0, 0,                    0, 1
        ) 
        * math::mat4x4(
            std::cos(rotation.z), -std::sin(rotation.z), 0, 0,
            std::sin(rotation.z),  std::cos(rotation.z), 0, 0,
                               0,                     0, 1, 0,
                               0,                     0, 0, 1
        );
    }
};

class Renderer
{
public:
    Renderer(const utils::SharedPtr<gfx::GraphicAPI>& api) : m_graphicAPI(api)
    {
        float fov = 60 * (PI / 180.0f);
        float aspectRatio = 800.0f / 600.0f;
        float zNear = 0.1f;
        float zFar = 100;

        float ys = 1 / std::tan(fov * 0.5);
        float xs = ys / aspectRatio;
        float zs = zFar / (zFar - zNear);

        m_projectionMatrix = math::mat4x4 (xs,   0,  0,           0,
                                            0,  ys,  0,           0,
                                            0,   0, zs, -zNear * zs,
                                            0,   0,  1,           0);

    }

    void useMaterial(const Material& mat)
    {
        m_graphicAPI->useGraphicsPipeline(mat.graphicPipeline);
        for (auto& texture : mat.textures)
            m_graphicAPI->setFragmentTexture(mat.graphicPipeline->findFragmentUniformIndex(texture.first), texture.second);
    }

    void renderObject(const Object& obj)
    {
        useMaterial(obj.material);
        m_graphicAPI->useVertexBuffer(obj.vertexBuffer);
        m_graphicAPI->setVertexUniform(obj.material.graphicPipeline->findVertexUniformIndex("u_MVPMatrix"), m_projectionMatrix * obj.modelMatrix());
        m_graphicAPI->drawIndexedVertices(obj.indexBuffer);
    }

private:
    math::mat4x4 m_projectionMatrix;
    utils::SharedPtr<gfx::GraphicAPI> m_graphicAPI;
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

    Object cube;
    cube.vertexBuffer = graphicAPI->newVertexBuffer<Vertex>(
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

    cube.indexBuffer = graphicAPI->newIndexBuffer(
    {
         0,  1,  2,  0,  2,  3,
         4,  5,  6,  4,  6,  7,
         8,  9, 10,  8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23
    });

    cube.material.graphicPipeline = graphicAPI->newGraphicsPipeline(makeGfxPipelineDescriptor("texturedCube"));
    
    cube.material.textures["u_texture"] = textureFromFile(graphicAPI, RESSOURCES_DIR"/mc_grass.jpg");

    Renderer renderer(graphicAPI);

    bool running = true;

    window->addEventCallBack([&](gfx::Event& event)
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

        renderer.renderObject(cube);

        graphicAPI->endOnScreenRenderPass();
        graphicAPI->endFrame();
    }

    gfx::Platform::terminate();
}