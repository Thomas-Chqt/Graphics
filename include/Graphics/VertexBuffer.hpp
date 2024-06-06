/*
 * ---------------------------------------------------
 * VertexBuffer.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/20 14:24:33
 * ---------------------------------------------------
 */

#ifndef VERTEXBUFFER_HPP
# define VERTEXBUFFER_HPP

#ifdef GFX_OPENGL_ENABLED
    #include "UtilsCPP/Array.hpp"
    #include <GL/glew.h>
    #include <cassert>
#endif
#include "UtilsCPP/Types.hpp"

namespace gfx
{

class VertexBuffer
{
public:
    class LayoutBase
    {
    public:
        #ifdef GFX_OPENGL_ENABLED
        struct Element { GLint size; GLenum type; GLboolean normalized; GLsizei stride; const void * pointer; };
        #endif

        #ifdef GFX_OPENGL_ENABLED
        virtual const utils::Array<Element>& getElements() const = 0;
        #endif
        virtual utils::uint64 getSize() const = 0;
    };

    template<typename T>
    class Layout : public LayoutBase
    {
    public:
        #ifdef GFX_OPENGL_ENABLED
        #if defined(__clang__) && __clang_major__ >= 18 
            inline const utils::Array<Element>& getElements() const override { static_assert(false, "Need to be define in a template specialization"); };
        #else
            inline const utils::Array<Element>& getElements() const override { assert(false && "Need to be define in a template specialization"); };
        #endif
        #endif
        inline utils::uint64 getSize() const override { return sizeof(T); };
    };

public:
    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer(VertexBuffer&&)      = delete;

    virtual ~VertexBuffer() = default;

protected:
    VertexBuffer() = default;

public:
    VertexBuffer& operator = (const VertexBuffer&) = delete;
    VertexBuffer& operator = (VertexBuffer&&)      = delete;
};

}

#endif // VERTEXBUFFER_HPP