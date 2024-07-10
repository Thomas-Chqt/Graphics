/*
 * ---------------------------------------------------
 * MetalBuffer.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/09 14:26:50
 * ---------------------------------------------------
 */

#ifndef METALBUFFER_HPP
# define METALBUFFER_HPP

#include "Graphics/Buffer.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    template<typename T> using id = T*;

    class MTLDevice;
    class MTLBuffer;
#endif // __OBJC__


namespace gfx
{

class MetalBuffer : public Buffer
{
public:
    MetalBuffer(const MetalBuffer&) = delete;
    MetalBuffer(MetalBuffer&&)      = delete;

    MetalBuffer(const id<MTLDevice>&, const Buffer::Descriptor&);

    utils::uint64 size() override;

    inline const id<MTLBuffer> mtlBuffer() { return m_mtlBuffer; }
    
    ~MetalBuffer() override;

private:
    id<MTLBuffer> m_mtlBuffer;

public:
    MetalBuffer& operator = (const MetalBuffer&) = delete;
    MetalBuffer& operator = (MetalBuffer&&)      = delete;
};

}

#endif // METALBUFFER_HPP