/*
 * ---------------------------------------------------
 * loadFile.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/29 10:49:15
 * ---------------------------------------------------
 */

#ifndef LOADFILE_HPP
# define LOADFILE_HPP

#include "Graphics/GraphicAPI.hpp"
#include "Mesh.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/String.hpp"

utils::Array<Mesh> loadFile(gfx::GraphicAPI&, const utils::String& filePath);

#endif // LOADFILE_HPP