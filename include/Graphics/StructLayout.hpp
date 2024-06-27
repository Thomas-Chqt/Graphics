/*
 * ---------------------------------------------------
 * StructLayout.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/23 12:19:58
 * ---------------------------------------------------
 */

#ifndef STRUCTLAYOUT_HPP
# define STRUCTLAYOUT_HPP

#include "Enums.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/String.hpp"

namespace gfx
{

struct StructLayoutElement
{
    utils::String name;
    gfx::Type type;
    const void* offset;
};

using StructLayout = utils::Array<StructLayoutElement>;

template<typename T> extern StructLayout getLayout();

}

#endif // STRUCTLAYOUT_HPP