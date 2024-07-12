/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/06 16:59:24
 * ---------------------------------------------------
 */

#include "Graphics/Buffer.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Event.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/Sampler.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/VertexLayout.hpp"
#include "Graphics/Window.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"
#include <cstddef>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

struct Vertex
{
    math::vec2f pos;
    math::vec2f uv;
};

utils::SharedPtr<gfx::Shader> createVertexShader(const gfx::GraphicAPI& api)
{
    gfx::Shader::Descriptor shaderDescriptor;
    shaderDescriptor.type = gfx::ShaderType::vertex;
    #ifdef GFX_BUILD_METAL
        shaderDescriptor.mtlShaderLibPath = MTL_SHADER_LIB;
        shaderDescriptor.mtlFunction = "texturedSquare_vs";
    #endif
    #ifdef GFX_BUILD_OPENGL
        shaderDescriptor.openglCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/texturedSquare.vs");
    #endif
    return api.newShader(shaderDescriptor);
}

utils::SharedPtr<gfx::Shader> createFragmentShader(const gfx::GraphicAPI& api)
{
    gfx::Shader::Descriptor shaderDescriptor;
    shaderDescriptor.type = gfx::ShaderType::fragment;
    #ifdef GFX_BUILD_METAL
        shaderDescriptor.mtlShaderLibPath = MTL_SHADER_LIB;
        shaderDescriptor.mtlFunction = "texturedSquare_fs";
    #endif
    #ifdef GFX_BUILD_OPENGL
        shaderDescriptor.openglCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/texturedSquare.fs");
    #endif
    return api.newShader(shaderDescriptor);
}

utils::SharedPtr<gfx::GraphicPipeline> createGraphicPipeline(const gfx::GraphicAPI& api)
{
    gfx::VertexLayout vertexLayout;
    vertexLayout.attributes.append({gfx::VertexAttributeFormat::vec2f, 0});
    vertexLayout.attributes.append({gfx::VertexAttributeFormat::vec2f, offsetof(Vertex, uv)});
    vertexLayout.stride = sizeof(Vertex);

    gfx::GraphicPipeline::Descriptor graphicPipelineDescriptor;

    graphicPipelineDescriptor.vertexLayout = vertexLayout;

    utils::SharedPtr<gfx::Shader> vertexShader = createVertexShader(api);
    graphicPipelineDescriptor.vertexShader = vertexShader;

    utils::SharedPtr<gfx::Shader> fragmentShader = createFragmentShader(api);
    graphicPipelineDescriptor.fragmentShader = fragmentShader;

    return api.newGraphicsPipeline(graphicPipelineDescriptor);
}

utils::SharedPtr<gfx::Buffer> createVertexBuffer(const gfx::GraphicAPI& api)
{
    const Vertex vertices[] = {
        { math::vec2f{-0.5, -0.5}, math::vec2f{ 0.0, 2.0 } },
        { math::vec2f{-0.5,  0.5}, math::vec2f{ 0.0, 0.0 } },
        { math::vec2f{ 0.5,  0.5}, math::vec2f{ 2.0, 0.0 } },
        { math::vec2f{ 0.5, -0.5}, math::vec2f{ 2.0, 2.0 } }
    };
    
    gfx::Buffer::Descriptor bufferDescriptor;
    bufferDescriptor.debugName = "vertexBuffer";
    bufferDescriptor.initialData = (const void*)vertices;
    bufferDescriptor.size = sizeof(vertices);
    
    return api.newBuffer(bufferDescriptor);
}

utils::SharedPtr<gfx::Buffer> createIndexBuffer(const gfx::GraphicAPI& api)
{
    const utils::uint32 indices[] = { 0, 1, 2, 0, 2, 3 };

    gfx::Buffer::Descriptor bufferDescriptor;
    bufferDescriptor.debugName = "indexBuffer";
    bufferDescriptor.initialData = (const void*)indices;
    bufferDescriptor.size = sizeof(indices);
    
    return api.newBuffer(bufferDescriptor);
}

utils::SharedPtr<gfx::Texture> createTexture(const gfx::GraphicAPI& api)
{
    int width = 0;
    int height = 0;
    stbi_uc* imgBytes = stbi_load(RESSOURCES_DIR"/mc_grass.jpg", &width, &height, nullptr, STBI_rgb_alpha);

    gfx::Texture::Descriptor textureDescriptor;
    textureDescriptor.width = width;
    textureDescriptor.height = height;

    utils::SharedPtr<gfx::Texture> texture = api.newTexture(textureDescriptor);
    texture->replaceContent(imgBytes);

    stbi_image_free(imgBytes);

    return texture;
}

utils::SharedPtr<gfx::Sampler> createSampler(const gfx::GraphicAPI& api)
{
    gfx::Sampler::Descriptor samplerDescriptor;

    samplerDescriptor.debugName = "mcGrassSampler";
    samplerDescriptor.sAddressMode = gfx::SamplerAddressMode::Repeat;
    samplerDescriptor.tAddressMode = gfx::SamplerAddressMode::Repeat;
    samplerDescriptor.minFilter = gfx::SamplerMinMagFilter::Linear;
    samplerDescriptor.magFilter = gfx::SamplerMinMagFilter::Linear;

    return api.newSampler(samplerDescriptor);
}

int main()
{
    gfx::Platform::init();
    {
        utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newWindow(WINDOW_WIDTH, WINDOW_HEIGHT); 
        utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newGraphicAPI(window);

        utils::SharedPtr<gfx::GraphicPipeline> graphicPipeline = createGraphicPipeline(*graphicAPI);

        utils::SharedPtr<gfx::Buffer> vertexBuffer = createVertexBuffer(*graphicAPI);
        utils::SharedPtr<gfx::Buffer> indexBuffer = createIndexBuffer(*graphicAPI);

        utils::SharedPtr<gfx::Texture> texture = createTexture(*graphicAPI);
        utils::SharedPtr<gfx::Sampler> sampler = createSampler(*graphicAPI);

        utils::uint64 textureBindingIndex = graphicPipeline->getFragmentTextureIndex("grass");
        utils::uint64 samplerBindingIndex = graphicPipeline->getFragmentSamplerIndex("texture_sampler");

        bool running = true;

        window->addEventCallBack([&](gfx::Event& event) {
            event.dispatch<gfx::KeyDownEvent>([&](gfx::KeyDownEvent& event) {
                if (event.keyCode() == ESC_KEY)
                    running = false;
            });
        });

        while (running)
        {
            gfx::Platform::shared().pollEvents();
            
            graphicAPI->beginFrame();
            {
                graphicAPI->beginRenderPass();
                {
                    graphicAPI->useGraphicsPipeline(graphicPipeline);

                    graphicAPI->setFragmentTexture(texture, textureBindingIndex, sampler, samplerBindingIndex);

                    graphicAPI->setVertexBuffer(vertexBuffer, 0);
                    graphicAPI->drawIndexedVertices(indexBuffer);
                }
                graphicAPI->endRenderPass();
            }
            graphicAPI->endFrame();
        }
    }
    gfx::Platform::terminate();
}