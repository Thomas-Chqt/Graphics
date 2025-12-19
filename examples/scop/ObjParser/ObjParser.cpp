/*
 * ---------------------------------------------------
 * ObjParser.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/12/11 08:30:57
 * ---------------------------------------------------
 */

#include "ObjParser/ObjParser.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <cassert>
#include <unordered_map>

namespace scop
{

ObjMesh parseObjFile(const std::filesystem::path& path)
{
    assert(std::filesystem::is_regular_file(path));

    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Failed to open OBJ file: " + path.string());

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> fileNormals;
    std::vector<uint32_t> indices;
    std::vector<glm::vec3> vertexNormals;
    bool normalsProvided = false;
    bool smoothingEnabled = true;

    auto resolveIndex = [](int idx, std::size_t size, const char* type) -> std::size_t {
        if (idx == 0)
            throw std::runtime_error(std::string("OBJ ") + type + " indices are 1-based and cannot be 0");
        if (idx > 0) {
            if (static_cast<std::size_t>(idx) > size)
                throw std::runtime_error(std::string("OBJ ") + type + " index out of range");
            return static_cast<std::size_t>(idx - 1);
        }

        auto absIdx = static_cast<std::size_t>(-idx);
        if (absIdx > size)
            throw std::runtime_error(std::string("Negative OBJ ") + type + " index out of range");
        return size - absIdx;
    };

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v")
        {
            float x = 0.0f, y = 0.0f, z = 0.0f;
            iss >> x >> y >> z;
            positions.emplace_back(x, y, z);
        }
        else if (prefix == "s")
        {
            std::string flag;
            iss >> flag;
            if (flag == "off" || flag == "0")
                smoothingEnabled = false;
            else
                smoothingEnabled = true;
        }
        else if (prefix == "vn")
        {
            float x = 0.0f, y = 0.0f, z = 0.0f;
            iss >> x >> y >> z;
            fileNormals.emplace_back(x, y, z);
        }
        else if (prefix == "f")
        {
            std::vector<std::size_t> faceIndices;
            std::string vertexToken;

            while (iss >> vertexToken)
            {
                std::size_t firstSlash = vertexToken.find('/');
                std::string posToken = firstSlash == std::string::npos ? vertexToken : vertexToken.substr(0, firstSlash);
                std::size_t resolvedPosIdx = resolveIndex(std::stoi(posToken), positions.size(), "position");

                std::size_t secondSlash = std::string::npos;
                bool hasNormalToken = false;
                std::size_t resolvedNormIdx = 0;
                if (firstSlash != std::string::npos) {
                    secondSlash = vertexToken.find('/', firstSlash + 1);
                    if (secondSlash != std::string::npos && secondSlash + 1 < vertexToken.size()) {
                        std::string normToken = vertexToken.substr(secondSlash + 1);
                        if (!normToken.empty()) {
                            hasNormalToken = true;
                            resolvedNormIdx = resolveIndex(std::stoi(normToken), fileNormals.size(), "normal");
                            normalsProvided = true;
                        }
                    }
                }

                if (hasNormalToken) {
                    if (vertexNormals.size() < positions.size())
                        vertexNormals.resize(positions.size(), glm::vec3(0.0f));
                    vertexNormals[resolvedPosIdx] += fileNormals[resolvedNormIdx];
                }

                faceIndices.push_back(resolvedPosIdx);
            }

            if (faceIndices.size() < 3)
                throw std::runtime_error("OBJ face with fewer than 3 vertices");

            for (std::size_t i = 1; i + 1 < faceIndices.size(); ++i)
            {
                indices.push_back(faceIndices[0]);
                indices.push_back(faceIndices[i]);
                indices.push_back(faceIndices[i+1]);
            }
        }
    }

    if (vertexNormals.empty() && !indices.empty())
    {
        if (!smoothingEnabled)
        {
            std::vector<glm::vec3> flatPositions;
            std::vector<glm::vec3> flatNormals;
            std::vector<std::uint32_t> flatIndices;
            flatPositions.reserve(indices.size());
            flatNormals.reserve(indices.size());
            flatIndices.reserve(indices.size());

            for (std::size_t i = 0; i + 2 < indices.size(); i += 3)
            {
                const auto i0 = indices[i];
                const auto i1 = indices[i + 1];
                const auto i2 = indices[i + 2];

                const glm::vec3 edge1 = positions[i1] - positions[i0];
                const glm::vec3 edge2 = positions[i2] - positions[i0];
                glm::vec3 faceNormal = glm::cross(edge1, edge2);
                const float length = glm::length(faceNormal);
                if (length == 0.0f)
                    continue;
                faceNormal *= (1.0f / length);

                const std::array<std::size_t, 3> corners{i0, i1, i2};
                for (std::size_t corner : corners)
                {
                    flatPositions.push_back(positions[corner]);
                    flatNormals.push_back(faceNormal);
                    flatIndices.push_back(static_cast<std::uint32_t>(flatPositions.size() - 1));
                }
            }

            positions = std::move(flatPositions);
            indices = std::move(flatIndices);
            vertexNormals = std::move(flatNormals);
        }
        else
        {
            vertexNormals.assign(positions.size(), glm::vec3(0.0f));
            for (std::size_t i = 0; i + 2 < indices.size(); i += 3)
            {
                const auto i0 = indices[i];
                const auto i1 = indices[i + 1];
                const auto i2 = indices[i + 2];

                const glm::vec3 edge1 = positions[i1] - positions[i0];
                const glm::vec3 edge2 = positions[i2] - positions[i0];
                glm::vec3 faceNormal = glm::cross(edge1, edge2);
                const float length = glm::length(faceNormal);
                if (length == 0.0f)
                    continue;
                faceNormal *= (1.0f / length);

                vertexNormals[i0] += faceNormal;
                vertexNormals[i1] += faceNormal;
                vertexNormals[i2] += faceNormal;
            }
        }
    }

    if (!vertexNormals.empty())
    {
        if (normalsProvided || !smoothingEnabled)
        {
            for (auto& n : vertexNormals)
            {
                const float length = glm::length(n);
                if (length != 0.0f)
                    n *= (1.0f / length);
            }
        }
        else
        {
            struct Vec3Key
            {
                float x;
                float y;
                float z;

                bool operator==(const Vec3Key&) const = default;
            };

            struct Vec3KeyHash
            {
                std::size_t operator()(const Vec3Key& k) const noexcept
                {
                    std::size_t h = 0;
                    std::hash<float> hf;
                    h ^= hf(k.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
                    h ^= hf(k.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
                    h ^= hf(k.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
                    return h;
                }
            };

            std::unordered_map<Vec3Key, glm::vec3, Vec3KeyHash> summedNormals;
            summedNormals.reserve(vertexNormals.size());

            for (std::size_t i = 0; i < positions.size(); ++i)
            {
                Vec3Key key{.x=positions[i].x, .y=positions[i].y, .z=positions[i].z};
                summedNormals[key] += vertexNormals[i];
            }

            for (std::size_t i = 0; i < positions.size(); ++i)
            {
                Vec3Key key{.x=positions[i].x, .y=positions[i].y, .z=positions[i].z};
                glm::vec3 n = summedNormals.at(key);
                const float length = glm::length(n);
                if (length != 0.0f)
                    n *= (1.0f / length);
                vertexNormals[i] = n;
            }
        }
    }

    return ObjMesh{
        .positions = std::move(positions),
        .indices = std::move(indices),
        .normals = vertexNormals.empty() ? std::nullopt : std::optional<std::vector<glm::vec3>>(std::move(vertexNormals))
    };
}

}
