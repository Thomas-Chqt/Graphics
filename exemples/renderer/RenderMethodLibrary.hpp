/*
 * ---------------------------------------------------
 * RenderMethodLibrary.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/30 09:33:39
 * ---------------------------------------------------
 */

#ifndef RENDERMETHODLIBRARY_HPP
# define RENDERMETHODLIBRARY_HPP

#include "Graphics/GraphicAPI.hpp"
#include "UtilsCPP/Dictionary.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/Types.hpp"
#include "UtilsCPP/UniquePtr.hpp"

enum class Shader { universal3D, baseTexture, baseColor };

class PointLight;
class Material;

class IRenderMethod
{
public:
    virtual void setVpMatrix(const math::mat4x4&) = 0;
    virtual void setModelMatrix(const math::mat4x4&) = 0;
    virtual void setCameraPos(const math::vec3f&) = 0;
    virtual void setPointLights(const utils::Array<const PointLight*>&) = 0;
    virtual void setMaterial(const Material&) = 0;

    virtual ~IRenderMethod() = default;

protected:
    IRenderMethod() = default;
};

template<Shader S>
void addToDescriptor(gfx::GraphicPipeline::Descriptor&);

template<Shader S>
void setVpMatrix(gfx::GraphicAPI&, const math::mat4x4&) {}

template<Shader S>
void setModelMatrix(gfx::GraphicAPI&, const math::mat4x4&) {}

template<Shader S>
void setCameraPos(gfx::GraphicAPI&, const math::vec3f&) {}

template<Shader S>
void setPointLights(gfx::GraphicAPI&, const utils::Array<const PointLight*>&) {}

template<Shader S>
void setMaterial(gfx::GraphicAPI&, const Material&) {}

template<Shader VS, Shader FS>
class RenderMethod : public IRenderMethod
{
public:
    RenderMethod(const utils::SharedPtr<gfx::GraphicAPI>& api) : m_graphicAPI(api)
    {
        gfx::GraphicPipeline::Descriptor descriptor;
        addToDescriptor<VS>(descriptor);
        addToDescriptor<FS>(descriptor);
        m_graphicPipeline = m_graphicAPI->newGraphicsPipeline(descriptor);
    }

    void setVpMatrix(const math::mat4x4& mat) override
    {
        setVpMatrix<VS>(mat);
    }

    void setModelMatrix(const math::mat4x4&) override
    {

    }

    void setCameraPos(const math::vec3f&) override
    {

    }

    void setPointLights(const utils::Array<const PointLight*>&) override
    {

    }

    void setMaterial(const Material&) override
    {

    }

    ~RenderMethod() override = default;

private:
    utils::SharedPtr<gfx::GraphicAPI> m_graphicAPI;
    utils::SharedPtr<gfx::GraphicPipeline> m_graphicPipeline;
};

class RenderMethodLibrary
{
public:
    static inline void init(const utils::SharedPtr<gfx::GraphicAPI>& api) { s_instance = utils::UniquePtr<RenderMethodLibrary>(new RenderMethodLibrary(api)); }
    static inline RenderMethodLibrary& shared() { return *s_instance; }
    static inline void terminate() { s_instance.clear(); }

    template<Shader VS, Shader FS>
    const utils::SharedPtr<IRenderMethod>& getRenderMethod()
    {
        return ;
    }

    ~RenderMethodLibrary() = default;

private:
    RenderMethodLibrary(const utils::SharedPtr<gfx::GraphicAPI>& api);

    template<Shader VS, Shader FS>
    utils::uint32 getRenderMethodID()
    {
        static utils::uint32 id = ++m_lastRenderMethodID;
        return id;
    }

    static utils::UniquePtr<RenderMethodLibrary> s_instance;

    utils::SharedPtr<gfx::GraphicAPI> m_api;
    utils::uint32 m_lastRenderMethodID = 0;
    utils::Dictionary<utils::uint32, utils::SharedPtr<IRenderMethod>> m_renderMethods;
};

#endif // RENDERMETHODLIBRARY_HPP