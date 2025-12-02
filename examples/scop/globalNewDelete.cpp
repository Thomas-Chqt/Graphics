/*
 * ---------------------------------------------------
 * globalNewDelete.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/12/02 08:45:31
 * ---------------------------------------------------
 */

#ifdef TRACY_ENABLE

#include <new>
#include <tracy/Tracy.hpp>

void* operator new(std::size_t size)
{
    void* ptr = std::malloc(size); // NOLINT(cppcoreguidelines-owning-memory, cppcoreguidelines-no-malloc)
    if (!ptr)
        throw std::bad_alloc();
    TracySecureAlloc(ptr, size);
    return ptr;
}

void* operator new[](std::size_t size)
{
    void* ptr = std::malloc(size); // NOLINT(cppcoreguidelines-owning-memory, cppcoreguidelines-no-malloc)
    if (!ptr)
        throw std::bad_alloc();
    TracySecureAlloc(ptr, size);
    return ptr;
}

void operator delete(void* ptr) noexcept
{
    TracySecureFree(ptr) ;
    std::free(ptr); // NOLINT(cppcoreguidelines-owning-memory, cppcoreguidelines-no-malloc)
}

void operator delete[](void* ptr) noexcept
{
    TracySecureFree(ptr) ;
    std::free(ptr); // NOLINT(cppcoreguidelines-owning-memory, cppcoreguidelines-no-malloc)
}

#endif
