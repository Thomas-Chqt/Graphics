/*
 * ---------------------------------------------------
 * RenderMethod.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/30 09:33:39
 * ---------------------------------------------------
 */

#ifndef RENDERMETHOD
# define RENDERMETHOD

#include "Graphics/GraphicAPI.hpp"
#include "UtilsCPP/Dictionary.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/Types.hpp"
#include "UtilsCPP/UniquePtr.hpp"

enum class VertexShader { universal3D };
enum class FragmentShader { universal, baseColor, baseTexture };

class PointLight;
class Material;

class IRenderMethod
{
public:
    virtual void use() = 0;

    virtual void setVpMatrix(const math::mat4x4&) = 0;
    virtual void setModelMatrix(const math::mat4x4&) = 0;
    virtual void setCameraPos(const math::vec3f&) = 0;
    virtual void setPointLights(const utils::Array<const PointLight*>&) = 0;
    virtual void setMaterial(const Material&) = 0;

    virtual ~IRenderMethod() = default;

protected:
    IRenderMethod() = default;
};

template<VertexShader VS> void addToDescriptor(gfx::GraphicPipeline::Descriptor&);
template<FragmentShader VS> void addToDescriptor(gfx::GraphicPipeline::Descriptor&);

template<VertexShader VS> void setVpMatrix(gfx::GraphicAPI&, const math::mat4x4&) {}
template<FragmentShader FS> void setVpMatrix(gfx::GraphicAPI&, const math::mat4x4&) {}

template<VertexShader VS> void setModelMatrix(gfx::GraphicAPI&, const math::mat4x4&) {}
template<FragmentShader FS> void setModelMatrix(gfx::GraphicAPI&, const math::mat4x4&) {}

template<VertexShader VS> void setCameraPos(gfx::GraphicAPI&, const math::vec3f&) {}
template<FragmentShader FS> void setCameraPos(gfx::GraphicAPI&, const math::vec3f&) {}

template<VertexShader VS> void setPointLights(gfx::GraphicAPI&, const utils::Array<const PointLight*>&) {}
template<FragmentShader FS> void setPointLights(gfx::GraphicAPI&, const utils::Array<const PointLight*>&) {}

template<VertexShader VS> void setMaterial(gfx::GraphicAPI&, const Material&) {}
template<FragmentShader FS> void setMaterial(gfx::GraphicAPI&, const Material&) {}

template<VertexShader VS, FragmentShader FS>
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

    void use() override
    {
        m_graphicAPI->useGraphicsPipeline(m_graphicPipeline);
    }

    void setVpMatrix(const math::mat4x4& mat) override
    {
        ::setVpMatrix<VS>(*m_graphicAPI, mat);
        ::setVpMatrix<FS>(*m_graphicAPI, mat);
    }

    void setModelMatrix(const math::mat4x4& mat) override
    {
        ::setModelMatrix<VS>(*m_graphicAPI, mat);
        ::setModelMatrix<FS>(*m_graphicAPI, mat);
    }

    void setCameraPos(const math::vec3f& vec) override
    {
        ::setCameraPos<VS>(*m_graphicAPI, vec);
        ::setCameraPos<FS>(*m_graphicAPI, vec);
    }

    void setPointLights(const utils::Array<const PointLight*>& lights) override
    {
        ::setPointLights<VS>(*m_graphicAPI, lights);
        ::setPointLights<FS>(*m_graphicAPI, lights);
    };

    void setMaterial(const Material& mat) override
    {
        ::setMaterial<VS>(*m_graphicAPI, mat);
        ::setMaterial<FS>(*m_graphicAPI, mat);
    }

    ~RenderMethod() override = default;

private:
    utils::SharedPtr<gfx::GraphicAPI> m_graphicAPI;
    utils::SharedPtr<gfx::GraphicPipeline> m_graphicPipeline;
};

class RenderMethodLibrary
{
public:
    inline void init(const utils::SharedPtr<gfx::GraphicAPI>& api) { s_instance = utils::UniquePtr<RenderMethodLibrary>(new RenderMethodLibrary(api)); }
    inline RenderMethodLibrary& shared() { return *s_instance; }
    inline void terminate() { s_instance->terminate(); }

    template<VertexShader VS, FragmentShader FS>
    utils::SharedPtr<IRenderMethod> renderMethod()
    {
        if(m_renderMethods.contain(renderMethodID<VS, FS>()))
            return m_renderMethods[renderMethodID<VS, FS>()];
        utils::SharedPtr<IRenderMethod> newRenderMethod = utils::SharedPtr<IRenderMethod>(new RenderMethod<VS, FS>(m_graphicAPI));
        m_renderMethods.insert(renderMethodID<VS, FS>(), newRenderMethod);
        return newRenderMethod;
    }

private:
    inline RenderMethodLibrary(const utils::SharedPtr<gfx::GraphicAPI>& api) : m_graphicAPI(api) {}

    template<VertexShader VS, FragmentShader FS>
    utils::uint32 renderMethodID()
    {
        static utils::uint32 id = m_lastRenderMethodID++;
        return id;
    }

    static utils::UniquePtr<RenderMethodLibrary> s_instance;

    utils::SharedPtr<gfx::GraphicAPI> m_graphicAPI;
    utils::uint32 m_lastRenderMethodID = 0;
    utils::Dictionary<utils::uint32, utils::SharedPtr<IRenderMethod>> m_renderMethods;
};


#endif // RENDERMETHOD