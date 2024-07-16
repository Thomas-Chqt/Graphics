/*
 * ---------------------------------------------------
 * RenderMethod.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/15 10:37:10
 * ---------------------------------------------------
 */

#ifndef RENDERMETHOD_HPP
# define RENDERMETHOD_HPP

#include "DirectionalLight.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Material.hpp"
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/SharedPtr.hpp"

class RenderMethod
{
public:
    RenderMethod()                    = delete;
    RenderMethod(const RenderMethod&) = delete;
    RenderMethod(RenderMethod&&)      = delete;

    virtual void use() = 0;

    virtual void setVpMatrix(math::mat4x4) = 0;
    virtual void setModelMatrix(math::mat4x4) = 0;
    virtual void setMaterial(const ::Material&) = 0;
    virtual void setLight(const ::DirectionalLight&) = 0;

    virtual ~RenderMethod() = default;

protected:
    RenderMethod(utils::SharedPtr<gfx::GraphicAPI>, utils::SharedPtr<gfx::GraphicPipeline>);

    utils::SharedPtr<gfx::GraphicAPI> m_api;
    utils::SharedPtr<gfx::GraphicPipeline> m_graphicPipeline;

public:
    RenderMethod& operator = (const RenderMethod&) = delete;
    RenderMethod& operator = (RenderMethod&&)      = delete;
};

class PhongRenderMethod : public RenderMethod
{
public:
    struct Vertex
    {
        math::vec3f pos;
        math::vec2f uv;
        math::vec3f normal;
        math::vec3f tangent;
    };

public:
    PhongRenderMethod()                         = delete;
    PhongRenderMethod(const PhongRenderMethod&) = delete;
    PhongRenderMethod(PhongRenderMethod&&)      = delete;

    PhongRenderMethod(const utils::SharedPtr<gfx::GraphicAPI>&);

    void use() override;

    void setVpMatrix(math::mat4x4) override;
    void setModelMatrix(math::mat4x4) override;
    void setMaterial(const ::Material&) override;
    void setLight(const ::DirectionalLight&) override;

    ~PhongRenderMethod() override = default;

private:
    struct Matrices
    {
        math::mat4x4 vpMatrix;
        math::mat4x4 modelMatrix;
    };

    struct Material
    {
        math::rgb diffuseColor;
        math::rgb specularColor;
        math::rgb emissiveColor;
        float shininess;

        int useDiffuseTexture;
        int useNormalMap;
    };

    struct DirectionalLight
    {
        math::vec3f color;
        float ambiantIntensity;
        float diffuseIntensity;
        float specularIntensity;
        math::vec3f direction;
    };

    utils::SharedPtr<gfx::Buffer> m_matrixBuffer;
    utils::SharedPtr<gfx::Buffer> m_materialBuffer;
    utils::SharedPtr<gfx::Buffer> m_lightBuffer;

public:
    PhongRenderMethod& operator = (const PhongRenderMethod&) = delete;
    PhongRenderMethod& operator = (PhongRenderMethod&&)      = delete;
};

#endif // RENDERMETHOD_HPP
