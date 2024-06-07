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
#include "Graphics/ShaderLibrary.hpp"

#include "Graphics/Texture.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "Graphics/Window.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"

#include "Vertex.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 

int main()
{
    gfx::Platform::init();
    gfx::ShaderLibrary::init();

    #ifdef GFX_METAL_ENABLED
        gfx::ShaderLibrary::shared().setMetalShaderLibPath(MTL_SHADER_LIB);
    #endif

    gfx::ShaderLibrary::shared().registerShader("texturedSquare_vs"
        #if GFX_METAL_ENABLED
            , "texturedSquare_vs"
        #endif
        #if GFX_OPENGL_ENABLED
            , utils::String::contentOfFile(OPENGL_SHADER_DIR"/texturedSquare.vs")
        #endif
    );
    gfx::ShaderLibrary::shared().registerShader("texturedSquare_fs"
        #if GFX_METAL_ENABLED
            , "texturedSquare_fs"
        #endif
        #if GFX_OPENGL_ENABLED
            , utils::String::contentOfFile(OPENGL_SHADER_DIR"/texturedSquare.fs")
        #endif
    );

    const utils::Array<Vertex> vertices = {
        { math::vec2f{-0.5, -0.5}, math::vec2f{ 0.0, 0.0 } },
        { math::vec2f{-0.5,  0.5}, math::vec2f{ 0.0, 1.0 } },
        { math::vec2f{ 0.5,  0.5}, math::vec2f{ 1.0, 1.0 } },
        { math::vec2f{ 0.5, -0.5}, math::vec2f{ 1.0, 0.0 } }
    };

    const utils::Array<utils::uint32> indices = utils::Array<utils::uint32>({ 0, 1, 2, 0, 2, 3 });


    utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newDefaultWindow(800, 600);
    utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newDefaultGraphicAPI(window);

    utils::SharedPtr<gfx::GraphicPipeline> graphicPipeline = graphicAPI->newGraphicsPipeline("texturedSquare_vs", "texturedSquare_fs");
    utils::SharedPtr<gfx::VertexBuffer> vertexBuffer = graphicAPI->newVertexBuffer(vertices);
    utils::SharedPtr<gfx::IndexBuffer> indexBuffer = graphicAPI->newIndexBuffer(indices);

    int width;
    int height;
    stbi_set_flip_vertically_on_load(true);
    stbi_uc* imgBytes = stbi_load(RESSOURCES_DIR"/mc_grass.jpg", &width, &height, nullptr, STBI_rgb_alpha);
    utils::SharedPtr<gfx::Texture> texture = graphicAPI->newTexture(width, height);
    texture->setBytes(imgBytes);
    stbi_image_free(imgBytes);

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
        graphicAPI->setFragmentTexture(graphicPipeline->findFragmentUniformIndex("u_texture"), texture);
        graphicAPI->drawIndexedVertices(indexBuffer);

        graphicAPI->endFrame();
    }

    gfx::ShaderLibrary::terminated();
    gfx::Platform::terminate();
}