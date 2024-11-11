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
#include "Graphics/Shader.hpp"
#include "Graphics/VertexLayout.hpp"
#include "Graphics/Window.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

utils::SharedPtr<gfx::Buffer> createVertexBuffer(const gfx::GraphicAPI& api)
{
    const float vertices[6] = { -1, -1, 0, 1, 1, -1 };
    
    gfx::Buffer::Descriptor bufferDescriptor;
    bufferDescriptor.debugName = "vertexBuffer";
    bufferDescriptor.initialData = (const void*)vertices;
    bufferDescriptor.size = sizeof(float) * 6;
    
    return api.newBuffer(bufferDescriptor);
}

utils::SharedPtr<gfx::Shader> createVertexShader(const gfx::GraphicAPI& api)
{
    gfx::Shader::Descriptor shaderDescriptor;
    shaderDescriptor.type = gfx::ShaderType::vertex;
    #ifdef GFX_BUILD_METAL
        shaderDescriptor.mtlShaderLibPath = MTL_SHADER_LIB;
        shaderDescriptor.mtlFunction = "triangle_vs";
    #endif
    #ifdef GFX_BUILD_OPENGL
        shaderDescriptor.openglCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/triangle.vs");
    #endif

    return api.newShader(shaderDescriptor);
}

utils::SharedPtr<gfx::Shader> createFragmentShader(const gfx::GraphicAPI& api)
{
    gfx::Shader::Descriptor shaderDescriptor;
    shaderDescriptor.type = gfx::ShaderType::fragment;
    #ifdef GFX_BUILD_METAL
        shaderDescriptor.mtlShaderLibPath = MTL_SHADER_LIB;
        shaderDescriptor.mtlFunction = "triangle_fs";
    #endif
    #ifdef GFX_BUILD_OPENGL
        shaderDescriptor.openglCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/triangle.fs");
    #endif

    return api.newShader(shaderDescriptor);
}

utils::SharedPtr<gfx::GraphicPipeline> createGraphicPipeline(const gfx::GraphicAPI& api)
{
    gfx::VertexLayout vertexLayout;
    vertexLayout.attributes.append({gfx::VertexAttributeFormat::vec2f, 0});
    vertexLayout.stride = sizeof(float) * 2;

    gfx::GraphicPipeline::Descriptor graphicPipelineDescriptor;

    graphicPipelineDescriptor.vertexLayout = vertexLayout;

    utils::SharedPtr<gfx::Shader> vertexShader = createVertexShader(api);
    graphicPipelineDescriptor.vertexShader = vertexShader;

    utils::SharedPtr<gfx::Shader> fragmentShader = createFragmentShader(api);
    graphicPipelineDescriptor.fragmentShader = fragmentShader;

    return api.newGraphicsPipeline(graphicPipelineDescriptor);
}

utils::SharedPtr<gfx::Buffer> createColorBuffer(const gfx::GraphicAPI& api)
{
    struct
    {
        math::vec3f red = RED3;
        math::vec3f green = GREEN3;
        math::vec3f blue = BLUE3;
    } colors;

    gfx::Buffer::Descriptor desc;
    desc.debugName = "color buffer";
    desc.initialData = &colors;
    desc.size = sizeof(colors);

    return api.newBuffer(desc);
}

int main()
{
    gfx::Platform::init();
    {
        utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newWindow(WINDOW_WIDTH, WINDOW_HEIGHT); 
        utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newGraphicAPI(window);

        utils::SharedPtr<gfx::Buffer> vertexBuffer = createVertexBuffer(*graphicAPI);
        utils::SharedPtr<gfx::GraphicPipeline> graphicPipeline = createGraphicPipeline(*graphicAPI);
        utils::SharedPtr<gfx::Buffer> colorBuffer = createColorBuffer(*graphicAPI);

        utils::uint64 colorBufferBindingPoint = graphicPipeline->getFragmentBufferIndex("colors");

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

                    graphicAPI->setVertexBuffer(vertexBuffer, 0);
                    graphicAPI->setFragmentBuffer(colorBuffer, colorBufferBindingPoint);

                    graphicAPI->drawVertices(0, 3);
                }
                graphicAPI->endRenderPass();
            }
            graphicAPI->endFrame();
        }
    }
    gfx::Platform::terminate();
}