/*
 * ---------------------------------------------------
 * Pipeline.hpp
 * ---------------------------------------------------
 */

#ifndef PIPELINE_HPP
#define PIPELINE_HPP

namespace gfx
{

class Pipeline
{
public:
    Pipeline(const Pipeline&) = delete;
    Pipeline(Pipeline&&) = delete;

    virtual ~Pipeline() = default;

protected:
    Pipeline() = default;

public:
    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline& operator=(Pipeline&&) = delete;
};

} // namespace gfx

#endif // PIPELINE_HPP
