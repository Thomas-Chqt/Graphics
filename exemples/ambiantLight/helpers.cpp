/*
 * ---------------------------------------------------
 * helpers.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/14 23:42:59
 * ---------------------------------------------------
 */

#include "helpers.hpp"
#include "Math/Constants.hpp"
#include "UtilsCPP/Types.hpp"
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

math::mat4x4 makeProjectionMatrix(const utils::SharedPtr<gfx::Window>& window)
{
    utils::uint32 w, h;
    window->getWindowSize(&w, &h);

    float fov = 60 * (PI / 180.0f);
    float aspectRatio = (float)w / (float)h;
    float zNear = 0.1f;
    float zFar = 100;

    float ys = 1 / std::tan(fov * 0.5);
    float xs = ys / aspectRatio;
    float zs = zFar / (zFar - zNear);

    return math::mat4x4 (xs,   0,  0,           0,
                          0,  ys,  0,           0,
                          0,   0, zs, -zNear * zs,
                          0,   0,  1,           0);
}