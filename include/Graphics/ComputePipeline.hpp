/*
 * ---------------------------------------------------
 * ComputePipeline.hpp
 * ---------------------------------------------------
 */

#ifndef COMPUTEPIPELINE_HPP
#define COMPUTEPIPELINE_HPP

#include "Graphics/Pipeline.hpp"
#include "Graphics/ParameterBlockLayout.hpp"
#include "Graphics/ShaderFunction.hpp"

#include <cstdint>
#include <memory>
#include <vector>

namespace gfx
{

class ComputePipeline : public Pipeline
{
public:
    struct Descriptor
    {
        ShaderFunction* computeShader = nullptr;

        uint32_t threadsPerThreadgroupX = 1;
        uint32_t threadsPerThreadgroupY = 1;
        uint32_t threadsPerThreadgroupZ = 1;

        std::vector<std::shared_ptr<ParameterBlockLayout>> parameterBlockLayouts;

        auto operator<=>(const Descriptor&) const = default;
    };

public:
    ComputePipeline(const ComputePipeline&) = delete;
    ComputePipeline(ComputePipeline&&) = delete;

    virtual ~ComputePipeline() = default;

protected:
    ComputePipeline() = default;

public:
    ComputePipeline& operator=(const ComputePipeline&) = delete;
    ComputePipeline& operator=(ComputePipeline&&) = delete;
};

} // namespace gfx

#endif // COMPUTEPIPELINE_HPP
