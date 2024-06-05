/*
 * ---------------------------------------------------
 * IMockCAMetalLayer.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/05 18:39:39
 * ---------------------------------------------------
 */

#ifndef IMOCKCAMETALLAYER_HPP
# define IMOCKCAMETALLAYER_HPP

#include <CoreGraphics/CGGeometry.h>

namespace gfx_tests
{

class IMockCAMetalLayer
{
public:
    IMockCAMetalLayer();
    virtual ~IMockCAMetalLayer();

    virtual CGSize getDrawableSize() = 0;

    void* caMetalLayer;
};

class IMockCAMetalLayerStaticFuncs
{
public:
    virtual IMockCAMetalLayer* layer() = 0;

    static IMockCAMetalLayerStaticFuncs* s_instance;
};

}

#endif // IMOCKCAMETALLAYER_HPP