/*
 * ---------------------------------------------------
 * helpers.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/14 23:39:58
 * ---------------------------------------------------
 */

#ifndef UTILS_HPP
# define UTILS_HPP

#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/IndexBuffer.hpp"
#include "Graphics/Window.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"

utils::SharedPtr<gfx::GraphicPipeline> makePipeline(utils::SharedPtr<gfx::GraphicAPI> api, const utils::String& shaderName);
utils::SharedPtr<gfx::Texture> textureFromFile(const utils::SharedPtr<gfx::GraphicAPI>& api, const utils::String& path);
math::mat4x4 makeProjectionMatrix(const utils::SharedPtr<gfx::Window>& window);

struct SubMesh
{
    utils::SharedPtr<gfx::VertexBuffer> vertexBuffer;
    utils::SharedPtr<gfx::IndexBuffer> indexBuffer;
    utils::SharedPtr<gfx::Texture> texture;
};

utils::Array<SubMesh> loadModel(const utils::SharedPtr<gfx::GraphicAPI>& api, const utils::String& filePath);

#endif // UTILS_HPP