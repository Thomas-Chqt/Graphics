/*
 * ---------------------------------------------------
 * GraphicAPI.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/19 13:57:02
 * ---------------------------------------------------
 */

#ifndef GRAPHICAPI_HPP
# define GRAPHICAPI_HPP

#include "UtilsCPP/SharedPtr.hpp"
#include "Window.hpp"

#ifdef USING_APPKIT
    #define newDefaultGraphicAPI(target) newMetalGraphicAPI(target)
#else
    #define newDefaultGraphicAPI(target) newOpenGLGraphicAPI(target)
#endif

namespace gfx
{

class GraphicAPI
{
public:
    GraphicAPI(const GraphicAPI&) = delete;
    GraphicAPI(GraphicAPI&&)      = delete;

#ifdef USING_OPENGL
    static utils::SharedPtr<GraphicAPI> newOpenGLGraphicAPI(const utils::SharedPtr<Window>& renderTarget = utils::SharedPtr<Window>());
#endif
#ifdef USING_METAL
    static utils::SharedPtr<GraphicAPI> newMetalGraphicAPI(const utils::SharedPtr<Window>& renderTarget = utils::SharedPtr<Window>());
#endif

    virtual void setRenderTarget(const utils::SharedPtr<Window>&) = 0;

    virtual void setClearColor(float r, float g, float b, float a) = 0;    

    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    
    virtual ~GraphicAPI() = default;

protected:
    GraphicAPI() = default;

public:
    GraphicAPI operator = (const GraphicAPI&) = delete;
    GraphicAPI operator = (GraphicAPI&&)      = delete;
};

}

#endif // GRAPHICAPI_HPP