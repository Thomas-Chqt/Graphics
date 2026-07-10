/*
 * ---------------------------------------------------
 * BlitPassDescriptor.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2026/07/10
 * ---------------------------------------------------
 */

#ifndef BLITPASSDESCRIPTOR_HPP
#define BLITPASSDESCRIPTOR_HPP

namespace gfx
{

class BlitPassDescriptor
{
public:
    BlitPassDescriptor(const BlitPassDescriptor&) = delete;
    BlitPassDescriptor(BlitPassDescriptor&&) = delete;

    virtual ~BlitPassDescriptor() = default;

protected:
    BlitPassDescriptor() = default;

public:
    BlitPassDescriptor& operator=(const BlitPassDescriptor&) = delete;
    BlitPassDescriptor& operator=(BlitPassDescriptor&&) = delete;
};

} // namespace gfx

#endif // BLITPASSDESCRIPTOR_HPP
