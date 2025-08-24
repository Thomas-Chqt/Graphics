/*
 * ---------------------------------------------------
 * Enums.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/30 16:50:31
 * ---------------------------------------------------
 */

#ifndef GFX_ENUMS_HPP
#define GFX_ENUMS_HPP

#if defined(GFX_USE_UTILSCPP)
#else
    #include <concepts>
    #include <type_traits>
    #include <cstdint>
    namespace ext = std;
#endif

namespace gfx
{

template<typename T>
    requires ext::is_enum_v<T> && ext::unsigned_integral<ext::underlying_type_t<T>>
class Flags
{
public:
    constexpr Flags() : m_value(0) {}

    constexpr Flags(T value) : m_value(static_cast<ext::underlying_type_t<T>>(value)) {}

    [[nodiscard]] constexpr inline T value() const { return m_value; }

    [[nodiscard]] constexpr inline bool operator==(const Flags& rhs) const { return m_value == rhs.m_value; }
    [[nodiscard]] constexpr inline bool operator!=(const Flags& rhs) const { return m_value != rhs.m_value; }
    [[nodiscard]] constexpr inline bool operator<(const Flags& rhs) const { return m_value < rhs.m_value; }

    [[nodiscard]] constexpr inline Flags operator|(const Flags& rhs) const { return Flags(m_value | rhs.m_value); }
    [[nodiscard]] constexpr inline Flags operator&(const Flags& rhs) const { return Flags(m_value & rhs.m_value); }
    [[nodiscard]] constexpr inline Flags operator^(const Flags& rhs) const { return Flags(m_value ^ rhs.m_value); }
    [[nodiscard]] constexpr inline Flags operator~() const { return Flags(~m_value); }

    constexpr inline Flags& operator|=(const Flags& rhs) { return m_value |= rhs.m_value, *this; }
    constexpr inline Flags& operator&=(const Flags& rhs) { return m_value &= rhs.m_value, *this; }
    constexpr inline Flags& operator^=(const Flags& rhs) { return m_value ^= rhs.m_value, *this; }

    constexpr inline explicit operator bool() const { return m_value != 0; }

private:
    constexpr Flags(ext::underlying_type_t<T> value) : m_value(value) {}

    ext::underlying_type_t<T> m_value;
};

template<typename E>
struct enable_bitmask_operators
{
    static constexpr bool enable = false;
};

#define GFX_ENABLE_BITMASK_OPERATORS(E) /* NOLINT */ \
    template<>                                       \
    struct enable_bitmask_operators<E>               \
    {                                                \
        static constexpr bool enable = true;         \
    };

template<typename E>
    requires ext::is_enum_v<E> && enable_bitmask_operators<E>::enable
[[nodiscard]] inline Flags<E> operator|(E lhs, E rhs)
{
    return Flags(lhs) | Flags(rhs);
}

template<typename E>
    requires ext::is_enum_v<E> && enable_bitmask_operators<E>::enable
[[nodiscard]] inline Flags<E> operator&(E lhs, E rhs)
{
    return Flags(lhs) & Flags(rhs);
}

template<typename E>
    requires ext::is_enum_v<E> && enable_bitmask_operators<E>::enable
[[nodiscard]] inline Flags<E> operator^(E lhs, E rhs)
{
    return Flags(lhs) ^ Flags(rhs);
}

template<typename E>
    requires ext::is_enum_v<E> && enable_bitmask_operators<E>::enable
[[nodiscard]] inline Flags<E> operator~(E val)
{
    return ~Flags(val);
}

enum class PixelFormat : uint8_t
{
    RGBA8Unorm,
    BGRA8Unorm,
    BGRA8Unorm_sRGB,
    Depth32Float
};

enum class PresentMode : uint8_t
{
    fifo,
    mailbox
};

enum class LoadAction : uint8_t
{
    load,
    clear
};

enum class BlendOperation : uint8_t
{
    blendingOff,
    srcA_plus_1_minus_srcA,
    one_minus_srcA_plus_srcA
};

enum class VertexAttributeFormat : uint8_t
{
    float2,
    float3
};

enum class BufferUsage : uint8_t
{
    vertexBuffer  = 1 << 0,
    indexBuffer   = 1 << 1,
    uniformBuffer = 1 << 2,
    perFrameData  = 1 << 3,  // one buffer per frame in flight TODO : remove (define trough other usages)
    copySource    = 1 << 4
};
GFX_ENABLE_BITMASK_OPERATORS(BufferUsage);
using BufferUsages = Flags<BufferUsage>;

enum class ResourceStorageMode : uint8_t
{
    deviceLocal,
    hostVisible,
};

enum class Backend : uint8_t
{
    metal  = 1 << 0,
    vulkan = 1 << 1
};
GFX_ENABLE_BITMASK_OPERATORS(Backend);
using Backends = Flags<Backend>;

enum class BindingType : uint8_t
{
    uniformBuffer,
    sampledTexture,
    sampler
};

enum class BindingUsage : uint8_t
{
    vertexRead    = 1 << 1,
    vertexWrite   = 1 << 2,
    fragmentRead  = 1 << 3,
    fragmentWrite = 1 << 4
};
GFX_ENABLE_BITMASK_OPERATORS(BindingUsage);
using BindingUsages = Flags<BindingUsage>;

enum class TextureUsage : uint8_t
{
    shaderRead             = 1 << 0,
    colorAttachment        = 1 << 1,
    depthStencilAttachment = 1 << 2,
    copyDestination        = 1 << 3
};
GFX_ENABLE_BITMASK_OPERATORS(TextureUsage);
using TextureUsages = Flags<TextureUsage>;

enum class TextureType : uint8_t
{
    texture2d
};

} // namespace gfx

#endif // GFX_ENUMS_HPP
