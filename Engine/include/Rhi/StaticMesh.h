// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"
#include "Rhi/Objects.h"
#include "Rhi/ResourceReference.h"
#include "Rhi/DeviceBuffers.h"
#include "Engine/Jxx.h"

namespace Jafg
{

struct LRenderInfo;
struct LMaterialInstance;

struct LStaticMesh;
typedef rhi::shared_ref<LStaticMesh> LStaticMeshRef;

typedef rhi::resource_state_bits EStaticMeshStateBits;
typedef rhi::resource_state_flags EStaticMeshState;

//# TODO: Make also an abstraction for instanced static meshes.
//# TODO: Abstract the size of indices (u16 vs u32). (Currently u32 only.)
struct LStaticMesh final
{
    struct Vertex final: rhi::vertex_input_template<Vertex>
    {
        LVec3F Position;
        LVec3F Normal;
        LVec2F TexCoord;
        LVec4F Tangent;

        NODISCARD static std::array<vk::VertexInputBindingDescription, 1> const& binding_descriptions() noexcept
        {
            static std::array Desc{vk::VertexInputBindingDescription{
                .binding = 0,
                .stride = sizeof(LStaticMesh::Vertex),
                .inputRate = vk::VertexInputRate::eVertex
                }};
            return Desc;
        }

        NODISCARD static std::array<vk::VertexInputAttributeDescription, 4> const& attribute_descriptions() noexcept
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
                    },
                vk::VertexInputAttributeDescription{
                    .location = 3, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(LStaticMesh::Vertex, Tangent)
                    },
                };
            return Desc;
        }

        FORCEINLINE constexpr bool operator==(Vertex const& V) const noexcept
        {
            return this->Position == V.Position && this->Normal == V.Normal && this->TexCoord == V.TexCoord && this->Tangent == V.Tangent;
        }
    };
    static_assert(rhi::vertex_input<Vertex>);

    struct HostPrimitive final
    {
        TArray<Vertex> Vertices;
        TArray<u32> Indices;
    };

    struct HostMesh final
    {
        LString Name;
        TArray<HostPrimitive> Primitives;
    };

    struct DevicePrimitive final
    {
        rhi::device_buffer Vertices;
        rhi::device_buffer Indices;
        u32 IndexCount;
    };

    struct DeviceMesh final
    {
        LString Name;
        TArray<DevicePrimitive> Primitives;
    };

    enum struct EResult
    {
        Success,
        FileNotFound,
        FailedToOpen,
        FailedToRead,
        LoadingError,
    };

    inline LStaticMesh() = default;
    inline LStaticMesh(LPath InPath, EStaticMeshState State = EStaticMeshStateBits::None) : Path{std::move(InPath)}
    {
        this->Reload(State);
    }
    PROHIBIT_REALLOC_OF_ANY_FORM(LStaticMesh)
    ~LStaticMesh() = default;

    //# Reload, due to corruption, etc. Usually not used outside the editor.
    void Reload(EStaticMeshState State = EStaticMeshStateBits::None)
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

#if JAFG_WITH_EDITOR
    void LoadDifferentMeshForAllClients(LPath Path, EStaticMeshState State = EStaticMeshStateBits::None)
    {
        this->Path = std::move(Path);
        this->FreeFromHost();
        this->FreeFromDevice();
        this->Reload(State);
    }
#endif /* JAFG_WITH_EDITOR */

    FORCEINLINE constexpr bool IsOnHost() const noexcept { return !this->HostMeshes.empty(); }
    ENGINE_API EResult LoadToHost();
    inline void FreeFromHost() noexcept { algo::orphan(&this->HostMeshes); }

    FORCEINLINE constexpr bool IsOnDevice() const noexcept { return !this->DeviceMeshes.empty(); }
    ENGINE_API void LoadToDevice();
    inline void FreeFromDevice() noexcept { this->DeviceMeshes.clear(); }

    ENGINE_API void Render(LActorRenderInfo const& Info, LWorldTrans const& Transform, LMaterialInstance const& Instance) const;
    ENGINE_API void DrawIndexed(LRenderInfo const& Info) const;

    NODISCARD FORCEINLINE constexpr LWorldAabb3 const& GetAabb() const noexcept { return this->Aabb; }
    NODISCARD FORCEINLINE constexpr LPath const& GetPath() const noexcept { return this->Path; }

    //# Host memory. Modify with care.
    TArray<HostMesh> HostMeshes;
    //# Device memory. Modify with care.
    /* TODO: This sucks. I am sure we can squish this into a single device buffer. */
    TArray<DeviceMesh> DeviceMeshes;

private:

    LWorldAabb3 Aabb{ maths::identity<LWorldAabb3> };
    LPath Path;
};

template<> ENGINE_API Detail::LNodeFactoryBase GetEditorNode<LStaticMeshRef>(TEditorNodeCreateInfo<LStaticMeshRef> const& Info) noexcept;

} /* ~Namespace Jafg */

template<>
struct std::hash<Jafg::LStaticMesh::Vertex>
{
    FORCEINLINE size_t operator()(Jafg::LStaticMesh::Vertex const& Vertex) const noexcept
    {
        return (hash<glm::vec3>()(Vertex.Position) >> 1) ^ (hash<glm::vec2>()(Vertex.TexCoord) << 1);
    }
};
