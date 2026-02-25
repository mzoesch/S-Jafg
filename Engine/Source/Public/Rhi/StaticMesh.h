// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Rhi/Rhi.h"
#include "Rhi/ResourceReference.h"

namespace Jafg
{

typedef EResourceStateBits EStaticMeshStateBits;
typedef EResourceState EStaticMeshState;

//# TODO: Make also an abstraction for instanced static meshes.
//# TODO: Abstract the size of indices (u16 vs u32). (Currently u32 only.)
struct LStaticMesh final
{
    struct Vertex final
    {
        LVec3F Position;
        LVec3F Normal;
        LVec2F TexCoord;

        static std::array<vk::VertexInputBindingDescription, 1> const& BindingDescriptions() noexcept
        {
            static std::array Desc{vk::VertexInputBindingDescription{
                .binding = 0,
                .stride = sizeof(LStaticMesh::Vertex),
                .inputRate = vk::VertexInputRate::eVertex
                }};
            return Desc;
        }

        static std::array<vk::VertexInputAttributeDescription, 3> const& AttributeDescriptions() noexcept
        {
            static std::array Desc{
                vk::VertexInputAttributeDescription{
                    .location = 0, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(LStaticMesh::Vertex, Position)
                    },
                vk::VertexInputAttributeDescription{
                    .location = 1, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(LStaticMesh::Vertex, Normal)
                    },
                vk::VertexInputAttributeDescription{
                    .location = 2, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = offsetof(LStaticMesh::Vertex, TexCoord)
                    }
                };
            return Desc;
        }

        FORCEINLINE constexpr bool operator==(LStaticMesh::Vertex const& V) const noexcept
        {
            return this->Position == V.Position && this->TexCoord == V.TexCoord;
        }
    };
    static_assert(CDeviceVertexInput<LStaticMesh::Vertex>);

    struct VPC final : public TVertexPushConstant<LStaticMesh::VPC>
    {
        LMat4F Model;
    };
    static_assert(CPushConstant<LStaticMesh::VPC>);

    enum struct EResult
    {
        Success,
        FileNotFound,
        LoadingError,
    };

    inline LStaticMesh() = default;
    inline LStaticMesh(LPath InPath, EStaticMeshState State = EStaticMeshStateBits::None) : Path{std::move(InPath)}
    {
        if (State & EStaticMeshStateBits::Host || State & EStaticMeshStateBits::Device)
        {
            auto Result{this->LoadToHost()};
            jassert(Result == EResult::Success)
        }

        if (State & EStaticMeshStateBits::Device)
        {
            this->LoadToDevice();
            if ((State & EStaticMeshStateBits::Host) == EStaticMeshStateBits::None)
            {
                this->FreeFromHost();
            }
        }

        return;
    }
    PROHIBIT_REALLOC_OF_ANY_FORM(LStaticMesh)
    ~LStaticMesh() = default;

    FORCEINLINE constexpr bool IsOnHost() const noexcept { return this->Vertices.size() > 0; }
    ENGINE_API EResult LoadToHost();
    inline void FreeFromHost() noexcept
    {
        algo::orphan(&this->Vertices);
        algo::orphan(&this->Indices);
    }

    FORCEINLINE constexpr bool IsOnDevice() const noexcept { return this->IndexCount > 0; }
    ENGINE_API void LoadToDevice();
    inline void FreeFromDevice() noexcept
    {
        this->IndexCount = 0;
        this->VertexBuffer.Free();
        this->IndexBuffer.Free();
    }

    ENGINE_API void DrawIndexed(LRenderInfo const& Info) const;

    FORCEINLINE constexpr LPath const& GetPath() const noexcept { return this->Path; }

    //# Modify with care.
    TArray<LStaticMesh::Vertex> Vertices;
    TArray<u32> Indices;

    //# Modify with care.
    u32 IndexCount{};
    LDeviceBuffer VertexBuffer;
    LDeviceBuffer IndexBuffer;

private:

    LPath Path;
};

typedef TSharedRef<LStaticMesh> LStaticMeshRef;

} /* ~Namespace Jafg */

template<>
struct std::hash<Jafg::LStaticMesh::Vertex>
{
    FORCEINLINE size_t operator()(Jafg::LStaticMesh::Vertex const& Vertex) const noexcept
    {
        return (hash<glm::vec3>()(Vertex.Position) >> 1) ^ (hash<glm::vec2>()(Vertex.TexCoord) << 1);
    }
};
