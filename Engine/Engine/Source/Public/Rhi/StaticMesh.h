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

//# TODO: Make also an abstraction for instanced static meshes.
//# TODO: Abstract the size of indices (u16 vs u32). (Currently u32 only.)
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

    // enum struct ELoadFlags : u8
    // {
    //     None = 0,
    //     LoadNormals = 1 << 0,
    //     LoadTangents = 1 << 1,
    // };
    enum struct EUploadBehavior
    {
        Immediate,
        Deferred,
    };
    enum struct EUploadHostMemoryBehavior
    {
        Free,
        Keep,
    };

    LStaticMesh() = default;
    LStaticMesh(LPath const& InPath) : Path{ InPath } { }

    inline EStaticMeshResult ReloadModel(
          EUploadBehavior Behavior = EUploadBehavior::Immediate
        , EUploadHostMemoryBehavior HostMemoryBehavior = EUploadHostMemoryBehavior::Free);

    inline void Upload(EUploadHostMemoryBehavior Behavior = EUploadHostMemoryBehavior::Free);

    inline void Render(LRenderInfo const& Info, LGraphicsDevicePipeline const& Pipeline) const
    {
        check( this->IndexCount > 0 )
        check( this->VertexBuffer.GetBuffer() && this->IndexBuffer.GetBuffer() )

        Info.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *Pipeline.Pipeline);

        Info.CommandBuffer.bindVertexBuffers(0, this->VertexBuffer.GetBuffer(), {0});
        Info.CommandBuffer.bindIndexBuffer(this->IndexBuffer.GetBuffer(), 0, vk::IndexTypeValue<decltype(this->Indices)::value_type>::value);




        Info.CommandBuffer.drawIndexed(static_cast<uint32_t>(this->IndexCount), 1, 0, 0, 0);

        return;
    }

    void FreeFromDevice()
    {
        this->IndexCount = 0;
        this->VertexBuffer.Free();
        this->IndexBuffer.Free();

        return;
    }

    LPath Path;

    TArray<LVertex> Vertices;
    TArray<u32> Indices;

    u32 IndexCount{ 0 };
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

#include "Rhi/StaticMeshImpl.h"
