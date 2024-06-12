/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/07 16:54:13
 * ---------------------------------------------------
 */

#include "Graphics/Event.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "Graphics/Window.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"

#include "UtilsCPP/String.hpp"
#include "Vertex.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 

template<>
utils::Array<gfx::VertexBuffer::LayoutElement> gfx::VertexBuffer::getLayout<Vertex>()
{
    return {
        { 2, Type::FLOAT, false, sizeof(Vertex), (void*)0 },
        { 2, Type::FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, uv) },
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
    stbi_set_flip_vertically_on_load(true);
    stbi_uc* imgBytes = stbi_load(path, &width, &height, nullptr, STBI_rgb_alpha);

    gfx::Texture::Descriptor textureDescriptor;
    textureDescriptor.width = width;
    textureDescriptor.height = height;

    utils::SharedPtr<gfx::Texture> texture = api->newTexture(textureDescriptor);
    texture->setBytes(imgBytes);

    stbi_image_free(imgBytes);

    return texture;
}

int main()
{
    gfx::Platform::init();

    const utils::Array<Vertex> vertices = {
        { math::vec2f{-0.5, -0.5}, math::vec2f{ 0.0, 0.0 } },
        { math::vec2f{-0.5,  0.5}, math::vec2f{ 0.0, 1.0 } },
        { math::vec2f{ 0.5,  0.5}, math::vec2f{ 1.0, 1.0 } },
        { math::vec2f{ 0.5, -0.5}, math::vec2f{ 1.0, 0.0 } }
    };

    const utils::Array<utils::uint32> indices = utils::Array<utils::uint32>({ 0, 1, 2, 0, 2, 3 });

    utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newDefaultWindow(800, 600);
    utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newDefaultGraphicAPI(window);

    #ifdef GFX_METAL_ENABLED
        graphicAPI->initMetalShaderLib(MTL_SHADER_LIB);
    #endif    

    utils::SharedPtr<gfx::GraphicPipeline> graphicPipeline = graphicAPI->newGraphicsPipeline(makeGfxPipelineDescriptor("texturedSquare"));
    utils::SharedPtr<gfx::VertexBuffer> vertexBuffer = graphicAPI->newVertexBuffer(vertices);
    utils::SharedPtr<gfx::IndexBuffer> indexBuffer = graphicAPI->newIndexBuffer(indices);
    utils::SharedPtr<gfx::Texture> grassTexture = textureFromFile(graphicAPI, RESSOURCES_DIR"/mc_grass.jpg");

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

    while (running)
    {
        gfx::Platform::shared().pollEvents();
        
        graphicAPI->beginFrame();
        
        graphicAPI->useGraphicsPipeline(graphicPipeline);
        graphicAPI->useVertexBuffer(vertexBuffer);
        graphicAPI->setFragmentTexture(graphicPipeline->findFragmentUniformIndex("u_texture"), grassTexture);
        graphicAPI->drawIndexedVertices(indexBuffer);

        graphicAPI->endFrame();
    }

    gfx::Platform::terminate();
}