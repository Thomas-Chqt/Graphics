#include "Graphics/Buffer.hpp"
#include "Graphics/CommandBuffer.hpp"
#include "Graphics/ComputePipeline.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Instance.hpp"
#include "Graphics/ParameterBlockLayout.hpp"
#include "Graphics/ParameterBlockPool.hpp"
#include "Graphics/ShaderLib.hpp"
#include "Graphics/Texture.hpp"

#include "compute.slang"

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#include <algorithm>
#include <array>
#include <charconv>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <format>
#include <limits>
#include <memory>
#include <numbers>
#include <print>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace
{

constexpr uint32_t threadsPerThreadgroupX = 8;
constexpr uint32_t threadsPerThreadgroupY = 8;
static_assert(sizeof(glm::vec2) == sizeof(float) * 2);

struct Options
{
    uint32_t width = 1024;
    uint32_t height = 1024;
    uint32_t iterations = 6000;

    float diffusionU = 0.16F;
    float diffusionV = 0.08F;
    float feed = 0.022F;
    float kill = 0.051F;
    float timeStep = 1.0F;

    uint32_t seedCount = 880;
    uint32_t seedRadius = 3;
    uint32_t randomSeed = 1337;

    std::filesystem::path output = "reaction_diffusion.png";
    bool showHelp = false;
};

void printUsage(std::string_view executable)
{
    std::println(
        "Gray-Scott reaction-diffusion on the GPU\n"
        "\n"
        "Usage: {} [options]\n"
        "\n"
        "  --width N          Simulation width (default: 1024)\n"
        "  --height N         Simulation height (default: 1024)\n"
        "  --iterations N     Number of GPU iterations (default: 6000)\n"
        "  --feed X           Feed rate F (default: 0.022)\n"
        "  --kill X           Kill rate K (default: 0.051)\n"
        "  --diffusion-u X    Diffusion rate for U (default: 0.16)\n"
        "  --diffusion-v X    Diffusion rate for V (default: 0.08)\n"
        "  --time-step X      Simulation time step (default: 1.0)\n"
        "  --seed-count N     Number of initial disturbances (default: 880)\n"
        "  --seed-radius N    Average disturbance radius (default: 3)\n"
        "  --random-seed N    Deterministic random seed (default: 1337)\n"
        "  --output PATH      Output PNG path (default: reaction_diffusion.png)\n"
        "  --help             Show this help",
        executable);
}

template<typename T>
T parseNumber(std::string_view text, std::string_view option)
{
    T value{};
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (error != std::errc{} || end != text.data() + text.size())
        throw std::invalid_argument(std::format("invalid value for {}: {}", option, text));
    return value;
}

Options parseOptions(int argc, char** argv)
{
    Options options;

    for (int i = 1; i < argc; ++i)
    {
        const std::string_view argument = argv[i];
        if (argument == "--help")
        {
            options.showHelp = true;
            continue;
        }

        const bool knownOption =
            argument == "--width" ||
            argument == "--height" ||
            argument == "--iterations" ||
            argument == "--feed" ||
            argument == "--kill" ||
            argument == "--diffusion-u" ||
            argument == "--diffusion-v" ||
            argument == "--time-step" ||
            argument == "--seed-count" ||
            argument == "--seed-radius" ||
            argument == "--random-seed" ||
            argument == "--output";
        if (!knownOption)
            throw std::invalid_argument(std::format("unknown option: {}", argument));

        if (i + 1 >= argc)
            throw std::invalid_argument(std::format("missing value after {}", argument));

        const std::string_view value = argv[++i];
        if (argument == "--width")
            options.width = parseNumber<uint32_t>(value, argument);
        else if (argument == "--height")
            options.height = parseNumber<uint32_t>(value, argument);
        else if (argument == "--iterations")
            options.iterations = parseNumber<uint32_t>(value, argument);
        else if (argument == "--feed")
            options.feed = parseNumber<float>(value, argument);
        else if (argument == "--kill")
            options.kill = parseNumber<float>(value, argument);
        else if (argument == "--diffusion-u")
            options.diffusionU = parseNumber<float>(value, argument);
        else if (argument == "--diffusion-v")
            options.diffusionV = parseNumber<float>(value, argument);
        else if (argument == "--time-step")
            options.timeStep = parseNumber<float>(value, argument);
        else if (argument == "--seed-count")
            options.seedCount = parseNumber<uint32_t>(value, argument);
        else if (argument == "--seed-radius")
            options.seedRadius = parseNumber<uint32_t>(value, argument);
        else if (argument == "--random-seed")
            options.randomSeed = parseNumber<uint32_t>(value, argument);
        else if (argument == "--output")
            options.output = value;
    }

    if (options.width < 8 || options.height < 8)
        throw std::invalid_argument("width and height must be at least 8");
    if (options.width > static_cast<uint32_t>(std::numeric_limits<int>::max() / 3) ||
        options.height > static_cast<uint32_t>(std::numeric_limits<int>::max()))
        throw std::invalid_argument("width and height exceed the PNG format limits");
    if (options.iterations == 0)
        throw std::invalid_argument("iterations must be greater than zero");
    if (options.seedCount == 0 || options.seedRadius == 0)
        throw std::invalid_argument("seed-count and seed-radius must be greater than zero");
    if (static_cast<uint64_t>(options.seedRadius) * 2 >= std::min(options.width, options.height))
        throw std::invalid_argument("seed-radius must be smaller than half of the grid");
    if (!std::isfinite(options.diffusionU) || options.diffusionU <= 0.0F ||
        !std::isfinite(options.diffusionV) || options.diffusionV <= 0.0F ||
        !std::isfinite(options.feed) || options.feed < 0.0F ||
        !std::isfinite(options.kill) || options.kill < 0.0F ||
        !std::isfinite(options.timeStep) || options.timeStep <= 0.0F)
        throw std::invalid_argument("simulation coefficients must be finite and non-negative; diffusion and time-step must be positive");
    if (options.output.empty())
        throw std::invalid_argument("output path must not be empty");

    return options;
}

std::vector<glm::vec2> makeInitialState(const Options& options)
{
    const size_t cellCount = static_cast<size_t>(options.width) * options.height;
    std::vector<glm::vec2> cells(cellCount, glm::vec2(1.0F, 0.0F));
    std::mt19937 random(options.randomSeed);
    std::uniform_real_distribution<float> unitFloat(0.0F, 1.0F);
    const glm::vec2 gridCenter(static_cast<float>(options.width) / 2.0F, static_cast<float>(options.height) / 2.0F);
    const float angleOffset = unitFloat(random) * 2.0F * std::numbers::pi_v<float>;
    constexpr float goldenAngle = 2.39996323F;

    for (uint32_t seedIndex = 0; seedIndex < options.seedCount; ++seedIndex)
    {
        const float radiusScale = 0.65F + unitFloat(random) * 0.7F;
        const uint32_t maxRadius = (std::min(options.width, options.height) - 1) / 2;
        const uint32_t radius = std::clamp(static_cast<uint32_t>(static_cast<float>(options.seedRadius) * radiusScale), 1u, maxRadius);

        auto centerX = static_cast<uint32_t>(gridCenter.x);
        auto centerY = static_cast<uint32_t>(gridCenter.y);
        if (seedIndex > 0)
        {
            const float angle = angleOffset + static_cast<float>(seedIndex) * goldenAngle;
            const glm::vec2 direction(std::cos(angle), std::sin(angle));
            const glm::vec2 lowerBound(static_cast<float>(radius));
            const glm::vec2 upperBound(static_cast<float>(options.width - radius - 1), static_cast<float>(options.height - radius - 1));
            const float distanceX = direction.x > 0.0F ? (upperBound.x - gridCenter.x) / direction.x : (lowerBound.x - gridCenter.x) / direction.x;
            const float distanceY = direction.y > 0.0F ? (upperBound.y - gridCenter.y) / direction.y : (lowerBound.y - gridCenter.y) / direction.y;
            const float radialProgress = std::sqrt(static_cast<float>(seedIndex) / static_cast<float>(options.seedCount - 1));
            const glm::vec2 center = gridCenter + direction * std::min(distanceX, distanceY) * radialProgress;
            centerX = std::clamp(static_cast<uint32_t>(std::lround(center.x)), radius, options.width - radius - 1);
            centerY = std::clamp(static_cast<uint32_t>(std::lround(center.y)), radius, options.height - radius - 1);
        }

        const uint32_t minX = centerX - radius;
        const uint32_t maxX = centerX + radius;
        const uint32_t minY = centerY - radius;
        const uint32_t maxY = centerY + radius;
        const uint64_t radiusSquared = static_cast<uint64_t>(radius) * radius;

        for (uint32_t y = minY; y <= maxY; ++y)
        {
            for (uint32_t x = minX; x <= maxX; ++x)
            {
                const int64_t dx = static_cast<int64_t>(x) - centerX;
                const int64_t dy = static_cast<int64_t>(y) - centerY;
                if (static_cast<uint64_t>(dx * dx + dy * dy) > radiusSquared)
                    continue;

                glm::vec2& cell = cells[static_cast<size_t>(y) * options.width + x];
                const float noise = (unitFloat(random) - 0.5F) * 0.08F;
                cell = glm::vec2(0.50F - noise, 0.25F + noise);
            }
        }
    }

    return cells;
}

glm::vec3 palette(float value)
{
    constexpr std::array colors{
        glm::vec3(0.012F, 0.020F, 0.075F),
        glm::vec3(0.105F, 0.055F, 0.310F),
        glm::vec3(0.520F, 0.080F, 0.400F),
        glm::vec3(0.930F, 0.300F, 0.220F),
        glm::vec3(1.000F, 0.780F, 0.260F),
        glm::vec3(0.980F, 0.960F, 0.820F)
    };

    const float scaled = glm::clamp(value, 0.0F, 1.0F) * static_cast<float>(colors.size() - 1);
    const size_t first = std::min(static_cast<size_t>(scaled), colors.size() - 2);
    return glm::mix(colors.at(first), colors.at(first + 1), scaled - static_cast<float>(first));
}

std::vector<uint8_t> makeImage(const glm::vec2* cells, size_t cellCount, float minV, float maxV)
{
    std::vector<uint8_t> image(cellCount * 3);
    const float range = std::max(maxV - minV, 0.000001F);

    for (size_t i = 0; i < cellCount; ++i)
    {
        const float value = glm::smoothstep(minV, minV + range, cells[i].y);
        const glm::vec3 color = palette(value);
        image[i * 3 + 0] = static_cast<uint8_t>(std::lround(color.r * 255.0F));
        image[i * 3 + 1] = static_cast<uint8_t>(std::lround(color.g * 255.0F));
        image[i * 3 + 2] = static_cast<uint8_t>(std::lround(color.b * 255.0F));
    }

    return image;
}

int run(const Options& options)
{
    const size_t cellCount = static_cast<size_t>(options.width) * options.height;
    if (cellCount > std::numeric_limits<size_t>::max() / sizeof(glm::vec2))
        throw std::invalid_argument("grid dimensions are too large");
    const size_t bufferSize = cellCount * sizeof(glm::vec2);

    std::vector<glm::vec2> initialState = makeInitialState(options);

    auto instance = gfx::Instance::newInstance(gfx::Instance::Descriptor{});

    auto device = instance->newDevice(gfx::Device::Descriptor{
        .queueCaps = {
            .graphics = false,
            .compute = true,
            .transfer = true,
            .present = {}
        }
    });

    std::unique_ptr<gfx::ShaderLib> shaderLib = device->newShaderLib(SHADER_SLIB);
    std::shared_ptr<gfx::ParameterBlockLayout> parameterBlockLayout = device->newParameterBlockLayout({
        .bindings = {
            {.type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::computeRead},
            {.type = gfx::BindingType::storageTexture, .usages = gfx::BindingUsage::computeWrite}
        }
    });

    std::shared_ptr<gfx::ComputePipeline> pipeline = device->newComputePipeline({
        .computeShader = &shaderLib->getFunction("simulate"),
        .threadsPerThreadgroupX = threadsPerThreadgroupX,
        .threadsPerThreadgroupY = threadsPerThreadgroupY,
        .threadsPerThreadgroupZ = 1,
        .parameterBlockLayouts = {parameterBlockLayout}
    });

    std::shared_ptr<gfx::Buffer> uploadBuffer = device->newBuffer({
        .size = bufferSize,
        .usages = gfx::BufferUsage::copySource,
        .storageMode = gfx::ResourceStorageMode::hostVisible
    });
    uploadBuffer->setContent(initialState.data(), bufferSize);

    const auto simulationTextureDescriptor = gfx::Texture::Descriptor{
        .type = gfx::TextureType::texture2d,
        .width = options.width,
        .height = options.height,
        .pixelFormat = gfx::PixelFormat::RG32Float,
        .usages = gfx::TextureUsage::shaderRead | gfx::TextureUsage::shaderWrite | gfx::TextureUsage::copySource | gfx::TextureUsage::copyDestination,
        .storageMode = gfx::ResourceStorageMode::deviceLocal
    };
    std::shared_ptr<gfx::Texture> textureA = device->newTexture(simulationTextureDescriptor);
    std::shared_ptr<gfx::Texture> textureB = device->newTexture(simulationTextureDescriptor);
    std::shared_ptr<gfx::Buffer> readbackBuffer = device->newBuffer({
        .size = bufferSize,
        .usages = gfx::BufferUsage::copyDestination,
        .storageMode = gfx::ResourceStorageMode::hostVisible
    });

    auto parameterBlockPool = device->newParameterBlockPool({
        .maxBindingCount = {
            {gfx::BindingType::sampledTexture, 2},
            {gfx::BindingType::storageTexture, 2}
        }
    });

    auto parameterBlockAB = parameterBlockPool->get(parameterBlockLayout);
    parameterBlockAB->setBinding(0, textureA);
    parameterBlockAB->setBinding(1, textureB);

    auto parameterBlockBA = parameterBlockPool->get(parameterBlockLayout);
    parameterBlockBA->setBinding(0, textureB);
    parameterBlockBA->setBinding(1, textureA);

    auto commandBuffer = device->newCommandBufferPool()->get();

    auto blitPass = device->newBlitPassDescriptor();
    commandBuffer->beginBlitPass(*blitPass);
    commandBuffer->copyBufferToTexture(uploadBuffer, textureA);
    commandBuffer->endBlitPass();

    auto computePass = device->newComputePassDescriptor();
    commandBuffer->beginComputePass(*computePass);
    commandBuffer->usePipeline(pipeline);
    const shader::compute::PushConstants pushConstants{
        .width = options.width,
        .height = options.height,
        .diffusionU = options.diffusionU,
        .diffusionV = options.diffusionV,
        .feed = options.feed,
        .kill = options.kill,
        .timeStep = options.timeStep
    };
    commandBuffer->setPushConstants(&pushConstants);

    const uint32_t groupCountX = (options.width + threadsPerThreadgroupX - 1) / threadsPerThreadgroupX;
    const uint32_t groupCountY = (options.height + threadsPerThreadgroupY - 1) / threadsPerThreadgroupY;
    for (uint32_t iteration = 0; iteration < options.iterations; ++iteration)
    {
        commandBuffer->setParameterBlock(iteration % 2 == 0 ? parameterBlockAB : parameterBlockBA, 0);
        commandBuffer->dispatchThreadgroups(groupCountX, groupCountY, 1);
    }
    commandBuffer->endComputePass();

    const std::shared_ptr<gfx::Texture>& finalTexture = options.iterations % 2 == 0 ? textureA : textureB;
    commandBuffer->beginBlitPass(*blitPass);
    commandBuffer->copyTextureToBuffer(finalTexture, readbackBuffer);
    commandBuffer->endBlitPass();

    std::println("Running {} x {} cells for {} iterations ({:.3f} billion cell updates)...",
                 options.width,
                 options.height,
                 options.iterations,
                 static_cast<double>(cellCount) * options.iterations / 1'000'000'000.0);

    const auto startTime = std::chrono::steady_clock::now();
    device->submitCommandBuffers(commandBuffer);
    device->waitCommandBuffer(*commandBuffer);
    const auto endTime = std::chrono::steady_clock::now();

    const auto* result = readbackBuffer->content<const glm::vec2>();
    float minU = std::numeric_limits<float>::max();
    float maxU = std::numeric_limits<float>::lowest();
    float minV = std::numeric_limits<float>::max();
    float maxV = std::numeric_limits<float>::lowest();
    double sumV = 0.0;
    for (size_t i = 0; i < cellCount; ++i)
    {
        minU = std::min(minU, result[i].x);
        maxU = std::max(maxU, result[i].x);
        minV = std::min(minV, result[i].y);
        maxV = std::max(maxV, result[i].y);
        sumV += result[i].y;
    }

    const std::vector<uint8_t> image = makeImage(result, cellCount, minV, maxV);
    const std::string outputPath = options.output.string();
    if (stbi_write_png(outputPath.c_str(),
                       static_cast<int>(options.width),
                       static_cast<int>(options.height),
                       3,
                       image.data(),
                       static_cast<int>(options.width * 3)) == 0)
    {
        throw std::runtime_error(std::format("failed to write PNG: {}", outputPath));
    }

    const std::chrono::duration<double, std::milli> elapsed = endTime - startTime;
    std::println("GPU execution: {:.2f} ms", elapsed.count());
    std::println("U range: [{:.5f}, {:.5f}]", minU, maxU);
    std::println("V range: [{:.5f}, {:.5f}], average: {:.5f}", minV, maxV, sumV / static_cast<double>(cellCount));
    std::println("Wrote {}", std::filesystem::absolute(options.output).string());

    return 0;
}

} // namespace

int main(int argc, char** argv)
{
    try
    {
        const Options options = parseOptions(argc, argv);
        if (options.showHelp)
        {
            printUsage(argc > 0 ? argv[0] : "compute");
            return 0;
        }
        return run(options);
    }
    catch (const std::exception& error)
    {
        std::println(stderr, "error: {}", error.what());
        std::println(stderr, "use --help to list the available parameters");
        return 1;
    }
}
