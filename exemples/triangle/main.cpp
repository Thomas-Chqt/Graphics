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

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

struct Vertex
{
    math::vec2f position;
};

utils::SharedPtr<gfx::Buffer> createVertexBuffer(const gfx::GraphicAPI& api, gfx::VertexLayout& layout)
{
    const utils::Array<Vertex> vertices = { {{-1, -1}}, {{0,  1}}, {{1, -1}} };
    
    layout.attributes.append({gfx::VertexAttributeFormat::vec2f, 0});
    layout.stride = sizeof(Vertex);

    gfx::Buffer::Descriptor bufferDescriptor;
    bufferDescriptor.debugName = "vertexBuffer";
    bufferDescriptor.initialData = (const void*)vertices;
    bufferDescriptor.size = sizeof(Vertex) * vertices.length();
    
    return api.newBuffer(bufferDescriptor);
}

utils::SharedPtr<gfx::Shader> createVertexShader(const gfx::GraphicAPI& api)
{
    gfx::Shader::MetalShaderDescriptor metalShaderDescriptor;
    #ifdef GFX_BUILD_METAL
        metalShaderDescriptor.type = gfx::ShaderType::vertex;
        metalShaderDescriptor.mtlShaderLibPath = MTL_SHADER_LIB;
        metalShaderDescriptor.mtlFunction = "triangle_vs";
    #endif

    gfx::Shader::OpenGLShaderDescriptor openglShaderDescriptor;
    #ifdef GFX_BUILD_OPENGL
        openglShaderDescriptor.type = gfx::ShaderType::vertex;
        openglShaderDescriptor.openglCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/triangle.vs");
    #endif

    return api.newShader(metalShaderDescriptor, openglShaderDescriptor);
}

utils::SharedPtr<gfx::Shader> createFragmentShader(const gfx::GraphicAPI& api)
{
    gfx::Shader::MetalShaderDescriptor metalShaderDescriptor;
    #ifdef GFX_BUILD_METAL
        metalShaderDescriptor.type = gfx::ShaderType::fragment;
        metalShaderDescriptor.mtlShaderLibPath = MTL_SHADER_LIB;
        metalShaderDescriptor.mtlFunction = "triangle_fs";
    #endif

    gfx::Shader::OpenGLShaderDescriptor openglShaderDescriptor;
    #ifdef GFX_BUILD_OPENGL
        openglShaderDescriptor.type = gfx::ShaderType::fragment;
        openglShaderDescriptor.openglCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/triangle.fs");
    #endif

    return api.newShader(metalShaderDescriptor, openglShaderDescriptor);
}

utils::SharedPtr<gfx::GraphicPipeline> createGraphicPipeline(const gfx::GraphicAPI& api, const gfx::VertexLayout& vertexLayout, const utils::SharedPtr<gfx::Shader>& vertexShader, const utils::SharedPtr<gfx::Shader>& fragmentShader)
{
    gfx::GraphicPipeline::Descriptor graphicPipelineDescriptor;
    graphicPipelineDescriptor.vertexLayout = vertexLayout;
    graphicPipelineDescriptor.vertexShader = vertexShader;
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
    } color;

    gfx::Buffer::Descriptor desc;
    desc.debugName = "color buffer";
    desc.initialData = &color;
    desc.size = sizeof(color);

    return api.newBuffer(desc);
}

int main()
{
    gfx::Platform::init();
    {
        utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newWindow(WINDOW_WIDTH, WINDOW_HEIGHT); 
        utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newGraphicAPI(window);

        gfx::VertexLayout vertexLayout;
        utils::SharedPtr<gfx::Buffer> vertexBuffer = createVertexBuffer(*graphicAPI, vertexLayout);
        utils::SharedPtr<gfx::Shader> vertexShader = createVertexShader(*graphicAPI);
        utils::SharedPtr<gfx::Shader> fragmentShader = createFragmentShader(*graphicAPI);
        utils::SharedPtr<gfx::GraphicPipeline> graphicPipeline = createGraphicPipeline(*graphicAPI, vertexLayout, vertexShader, fragmentShader);

        utils::SharedPtr<gfx::Buffer> colorBuffer = createColorBuffer(*graphicAPI);

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
            graphicAPI->beginRenderPass();
            
            graphicAPI->useGraphicsPipeline(graphicPipeline);

            graphicPipeline->bindBuffer(colorBuffer, "color");

            graphicAPI->useVertexBuffer(vertexBuffer);
            graphicAPI->drawVertices(0, 3);

            graphicAPI->endRenderPass();
            graphicAPI->endFrame();
        }
    }
    gfx::Platform::terminate();
}