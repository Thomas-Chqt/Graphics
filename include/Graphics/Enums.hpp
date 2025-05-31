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
    namespace ext = utl;
#else
    #include <cstdint>
    namespace ext = std;
#endif

namespace gfx
{

enum class QueueCapabilityFlag : uint32_t
{
    Graphics = 1 << 1,
    Compute = 1 << 2,
    Transfer = 1 << 3
};

inline QueueCapabilityFlag operator|(QueueCapabilityFlag a, QueueCapabilityFlag b)
{
    return static_cast<QueueCapabilityFlag>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline QueueCapabilityFlag operator&(QueueCapabilityFlag a, QueueCapabilityFlag b)
{
    return static_cast<QueueCapabilityFlag>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline QueueCapabilityFlag operator~(QueueCapabilityFlag a)
{
    return static_cast<QueueCapabilityFlag>(~static_cast<uint32_t>(a));
}

} // namespace gfx

#endif // GFX_ENUMS_HPP
