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
#include "Graphics/Window.hpp"
#include "UtilsCPP/SharedPtr.hpp"

utils::SharedPtr<gfx::GraphicPipeline> makePipeline(utils::SharedPtr<gfx::GraphicAPI> api, const utils::String& shaderName);
utils::SharedPtr<gfx::Texture> textureFromFile(const utils::SharedPtr<gfx::GraphicAPI>& api, const utils::String& path);
math::mat4x4 makeProjectionMatrix(const utils::SharedPtr<gfx::Window>& window);

#endif // UTILS_HPP