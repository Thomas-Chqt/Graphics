/*
 * ---------------------------------------------------
 * ShaderLibrary.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/25 16:02:26
 * ---------------------------------------------------
 */

#include "Graphics/ShaderLibrary.hpp"

namespace gfx
{

utils::UniquePtr<ShaderLibrary> ShaderLibrary::s_sharedInstance;

}