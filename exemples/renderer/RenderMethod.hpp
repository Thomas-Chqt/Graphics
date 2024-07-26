/*
 * ---------------------------------------------------
 * RenderMethod.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/17 16:28:43
 * ---------------------------------------------------
 */

#ifndef RENDERMETHOD_HPP
# define RENDERMETHOD_HPP

#include "Graphics/Buffer.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Math/Matrix.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/Types.hpp"
#include "Graphics/BufferInstance.hpp"

class Material;

class RenderMethod
{
public:
    struct MatrixBuffer
    {
        math::mat4x4 vpMatrix;
        math::mat4x4 modelMatrix;
    };

    struct LightBuffer
    {
        struct
        {
            math::vec3f color;
            float ambiantIntensity;
            float diffuseIntensity;
            float specularIntensity;
            math::vec3f direction;
        }
        directionalLights[32];
        utils::uint32 directionalLightCount;

        struct
        {
            math::vec3f color;
            float ambiantIntensity;
            float diffuseIntensity;
            float specularIntensity;
            math::vec3f position;
        }
        pointLights[32];
        utils::uint32 pointLightCount;
    };

    struct CameraBuffer
    {
        math::vec3f pos;
    };

public:
    RenderMethod()                    = delete;
    RenderMethod(const RenderMethod&) = delete;
    RenderMethod(RenderMethod&&)      = delete;

    virtual void use() = 0;

    virtual void setMatrixBuffer(const utils::SharedPtr<gfx::Buffer>&) = 0;
    virtual void setLightBuffer(const utils::SharedPtr<gfx::Buffer>&) = 0;
    virtual void setCameraBuffer(const utils::SharedPtr<gfx::Buffer>&) = 0;
    virtual void setMaterial(const Material&) = 0;

    virtual ~RenderMethod() = default;

protected:
    RenderMethod(utils::SharedPtr<gfx::GraphicAPI>, utils::SharedPtr<gfx::GraphicPipeline>);

    utils::SharedPtr<gfx::GraphicAPI> m_graphicAPI;
    utils::SharedPtr<gfx::GraphicPipeline> m_graphicPipeline;

public:
    RenderMethod& operator = (const RenderMethod&) = delete;
    RenderMethod& operator = (RenderMethod&&)      = delete;
};

class DefaultRenderMethod : public RenderMethod
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
    DefaultRenderMethod()                           = delete;
    DefaultRenderMethod(const DefaultRenderMethod&) = delete;
    DefaultRenderMethod(DefaultRenderMethod&&)      = delete;

    DefaultRenderMethod(const utils::SharedPtr<gfx::GraphicAPI>&);

    void use() override;

    void setMatrixBuffer(const utils::SharedPtr<gfx::Buffer>&) override;
    void setLightBuffer(const utils::SharedPtr<gfx::Buffer>&) override;
    void setCameraBuffer(const utils::SharedPtr<gfx::Buffer>&) override;
    void setMaterial(const Material&) override;

    ~DefaultRenderMethod() override = default;

private:
    struct MaterialBuffer
    {
        math::rgb diffuseColor;
        math::rgb specularColor;
        math::rgb emissiveColor;
        float shininess;

        int useDiffuseTexture;
        int useNormalMap;
        int useSpecularMap;
        int useEmissiveMap;
    };

    utils::uint64 m_matrixBufferIdx = 0;
    utils::uint64 m_lightBufferIdx = 0;
    utils::uint64 m_cameraBufferIdx = 0;

    gfx::BufferInstance<MaterialBuffer> m_materialBuffer;
    utils::uint64 m_materialBufferIdx = 0;

public:
    DefaultRenderMethod& operator = (const DefaultRenderMethod&) = delete;
    DefaultRenderMethod& operator = (DefaultRenderMethod&&)      = delete;
};

#endif // RENDERMETHOD_HPP