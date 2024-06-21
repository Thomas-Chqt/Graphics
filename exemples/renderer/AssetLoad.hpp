/*
 * ---------------------------------------------------
 * AssetLoad.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/21 17:34:00
 * ---------------------------------------------------
 */
#ifndef ASSETLOAD_HPP
# define ASSETLOAD_HPP

#include "Entity.hpp"
#include "Graphics/GraphicAPI.hpp"

RenderableEntity loadModel(const utils::SharedPtr<gfx::GraphicAPI>& api, const utils::String& filePath);
utils::SharedPtr<gfx::GraphicPipeline> makePipeline(utils::SharedPtr<gfx::GraphicAPI> api, const utils::String& shaderName);


#endif // ASSETLOAD_HPP