/*
 * ---------------------------------------------------
 * helpers.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/14 23:42:59
 * ---------------------------------------------------
 */

#include "helpers.hpp"
#include "stb_image.h"

utils::SharedPtr<gfx::GraphicPipeline> makePipeline(utils::SharedPtr<gfx::GraphicAPI> api, const utils::String& shaderName)
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

    return api->newGraphicsPipeline(graphicPipelineDescriptor);
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