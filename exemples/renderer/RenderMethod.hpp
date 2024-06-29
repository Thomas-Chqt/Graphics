/*
 * ---------------------------------------------------
 * RenderMethod.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/25 11:29:39
 * ---------------------------------------------------
 */

#ifndef RENDERMETHOD_HPP
# define RENDERMETHOD_HPP

#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"

enum class Shader { universal3D, baseTexture, baseColor };

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

template<Shader S>
void addToDescriptor(gfx::GraphicPipeline::Descriptor&);

template<Shader S>
void setVpMatrix(gfx::GraphicAPI&, const math::mat4x4&);

template<Shader S>
void setModelMatrix(gfx::GraphicAPI&, const math::mat4x4&);

template<Shader S>
void setCameraPos(gfx::GraphicAPI&, const math::vec3f&);

template<Shader S>
void setPointLights(gfx::GraphicAPI&, const utils::Array<const PointLight*>&);

template<Shader S>
void setMaterial(gfx::GraphicAPI&, const Material&);

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

    void use(const Uniforms& uniforms) override
    {
        m_graphicAPI->useGraphicsPipeline(m_graphicPipeline);
        setUniforms<VS>(*m_graphicAPI, uniforms);
        setUniforms<FS>(*m_graphicAPI, uniforms);
    }

    void setVpMatrix(const math::mat4x4&) override
    {

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

#endif // RENDERMETHOD_HPP