/*
 * ---------------------------------------------------
 * LoadFile.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/29 10:49:15
 * ---------------------------------------------------
 */

#ifndef LOADFILE_HPP
# define LOADFILE_HPP

#include "Entity.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/String.hpp"

utils::Array<RenderableEntity> loadFile(gfx::GraphicAPI&, const utils::String& filePath);

#endif // LOADFILE_HPP