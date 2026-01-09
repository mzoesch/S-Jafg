// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"
#include "Rhi/VkAl.h"

namespace Jafg
{

enum struct EStaticMeshResult
{
    Success,
    FileNotFound,
    LoadingError,
};

class LStaticMesh
{
public:

    struct LVertex
    {
        glm::vec3 Location;
        glm::vec3 Color;
        glm::vec2 TexCoord;

        static std::array<vk::VertexInputBindingDescription, 1> const& BindingDescriptions() noexcept
        {
            static std::array<vk::VertexInputBindingDescription, 1> Desc{vk::VertexInputBindingDescription{
                .binding = 0,
                .stride = sizeof(LVertex),
                .inputRate = vk::VertexInputRate::eVertex
                }};

            return Desc;
        }

        static std::array<vk::VertexInputAttributeDescription, 3> const& AttributeDescriptions() noexcept
        {
            static std::array<vk::VertexInputAttributeDescription, 3> Desc{
                vk::VertexInputAttributeDescription{
                    .location = 0, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(LVertex, Location)
                    },
                vk::VertexInputAttributeDescription{
                    .location = 1, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(LVertex, Color)
                    },
                vk::VertexInputAttributeDescription{
                    .location = 2, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = offsetof(LVertex, TexCoord)
                    }
                };

            return Desc;
        }

        FORCEINLINE constexpr bool operator==(LVertex const& V) const
        {
            return this->Location == V.Location && this->Color == V.Color && this->TexCoord == V.TexCoord;
        }
    };
    static_assert(Jafg::CDeviceVertexInput<LVertex>);

    LStaticMesh() = default;
    LStaticMesh(LPath const& InPath) : Path{ InPath } { }

    EStaticMeshResult ReloadModel();

    void UploadVertices()
    {
        check( this->Vertices.size() > 0 )
        check( this->Indices.size() > 0 )

        this->VertexBuffer = GEngine->GetLocalEgo().GetFrontend().Vk_StageBuffer(LStageBufferCreateInfo::Vertex({
            .BufferCopy = vk::BufferCopy{ 0, 0, sizeof(this->Vertices[0]) * this->Vertices.size() },
            .Data = this->Vertices.data(),
            }));

        this->IndexBuffer = GEngine->GetLocalEgo().GetFrontend().Vk_StageBuffer(LStageBufferCreateInfo::Index({
            .BufferCopy = vk::BufferCopy{ 0, 0, sizeof(this->Indices[0]) * this->Indices.size() },
            .Data = this->Indices.data(),
            }));

        return;
    }

    void FreeFromDevice()
    {
        this->VertexBuffer.Free();
        this->IndexBuffer.Free();

        return;
    }

    LPath Path;
    TArray<LVertex> Vertices;
    TArray<u32> Indices;

    LDeviceBuffer VertexBuffer;
    LDeviceBuffer IndexBuffer;
};

} /* ~Namespace Jafg */

template<>
struct std::hash<Jafg::LStaticMesh::LVertex>
{
    FORCEINLINE size_t operator()(Jafg::LStaticMesh::LVertex const& Vertex) const noexcept
    {
        return ((hash<glm::vec3>()(Vertex.Location) ^ (hash<glm::vec3>()(Vertex.Color) << 1)) >> 1) ^ (hash<glm::vec2>()(Vertex.TexCoord) << 1);
    }
};

namespace Jafg
{

inline EStaticMeshResult LStaticMesh::ReloadModel()
{
    LOG_VERBOSE(LogRhi, "Reloading static mesh from path [{}].", this->Path)

    if (Finder::DoesFileExist(this->Path) == false)
    {
        return EStaticMeshResult::FileNotFound;
    }

    tinyobj::attrib_t Attrib;
    std::vector<tinyobj::shape_t> Shapes;
    std::vector<tinyobj::material_t> Materials;
    LString Warning;
    LString Error;

    auto Result{ tinyobj::LoadObj(&Attrib, &Shapes, &Materials, &Warning, &Error, this->Path.c_str()) };
    if (Warning.empty() == false) { LOG_WARNING(LogRhi, "tinyobj: {}", Warning) }
    if (Result == false) { LOG_ERROR(LogRhi, "tinyobj: {}", Error) }
    if (Result == false) { return EStaticMeshResult::LoadingError; }

    std::unordered_map<LVertex, u32> UniqueVertices;

    algo::orphan(&this->Vertices);
    algo::orphan(&this->Indices);

    for (auto const& Shape : Shapes)
    {
        for (auto const& Idx : Shape.mesh.indices)
        {
            LVertex V;

            V.Location = {
                Attrib.vertices[3 * Idx.vertex_index + 0],
                Attrib.vertices[3 * Idx.vertex_index + 1],
                Attrib.vertices[3 * Idx.vertex_index + 2]
            };

            V.TexCoord = {
                Attrib.texcoords[2 * Idx.texcoord_index + 0],
                1.0f - Attrib.texcoords[2 * Idx.texcoord_index + 1]
            };

            V.Color = {1.0f, 1.0f, 1.0f};

            if (UniqueVertices.contains(V) == false)
            {
                UniqueVertices[V] = static_cast<uint32_t>(this->Vertices.size());
                this->Vertices.push_back(V);
            }

            Indices.push_back(UniqueVertices[V]);
        }
    }

    return EStaticMeshResult::Success;
}

} /* ~Namespace Jafg */
