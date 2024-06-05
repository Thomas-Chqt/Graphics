/*
 * ---------------------------------------------------
 * IMockNSWindow.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/05 16:53:29
 * ---------------------------------------------------
 */

#ifndef IMOCKNSWINDOW_HPP
# define IMOCKNSWINDOW_HPP

#include "QuartzCore/IMockCAMetalLayer.hpp"

namespace gfx_tests
{

class IMockNSView
{
public:
    IMockNSView();
    virtual ~IMockNSView();

    virtual void setWantsLayer(int) = 0;
    virtual IMockCAMetalLayer* getLayer() = 0;
    virtual void setLayer(void*) = 0;

    void* nsView;
};

class IMockNSWindow
{
public:
    IMockNSWindow();
    virtual ~IMockNSWindow();

    virtual IMockNSView* getContentView() = 0;

    void* nsWindow = nullptr;
};

}

#endif // IMOCKNSWINDOW_HPP