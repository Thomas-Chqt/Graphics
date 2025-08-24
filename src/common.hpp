/*
 * ---------------------------------------------------
 * common.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/11 16:47:18
 * ---------------------------------------------------
 */

#ifndef GFX_COMMON_HPP
#define GFX_COMMON_HPP

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <array>
    namespace ext = std;
#endif

constexpr int MAX_FRAME_IN_FLIGHT = 3;

template<typename T>
using PerFrameInFlight = ext::array<T, MAX_FRAME_IN_FLIGHT>;

#endif // GFX_COMMON_HPP
