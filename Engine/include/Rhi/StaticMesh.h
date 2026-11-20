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
struct LPbrMaterialInstance;

struct LStaticMesh;
typedef rhi::shared_ref<LStaticMesh> LStaticMeshRef;

//# @see #rhi::resource_state_bits
enum struct EStaticMeshStateBits
{
    None = 0 << 0,
    Host = 1 << 0,
    Device = 1 << 1,
    //#
    //# Create and keep a convex hull of the mesh on the host side.
    //# If not set, and you need to use the hull, the mesh has to be reloaded from secondary storage which might
    //# come with noticeable performance penalties.
    //#
    Convex = 1 << 2,

    PhysicsReady = Device | Convex,
};
ENUM_STRUCT_FLAGS(EStaticMeshStateBits, EStaticMeshState)

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
                    .location = 3, .binding = 0, .format = vk::Format::eR32G32B32A32Sfloat, .offset = offsetof(LStaticMesh::Vertex, Tangent)
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
        Empty,
        FileNotFound,
        FailedToOpen,
        FailedToRead,
        LoadingError,
    };

    inline LStaticMesh() = default;
    inline LStaticMesh(LPath InPath) : Path{std::move(InPath)} {}
    PROHIBIT_REALLOC_OF_ANY_FORM(LStaticMesh)
    ~LStaticMesh() = default;

    //# Brings the mesh to at least this state. If already in this state this is a noop.
    void BringToState(EStaticMeshState State);

#if JAFG_WITH_EDITOR
    //# Reload, due to corruption, etc. Usually not used outside the editor.
    void Reload(EStaticMeshState State);
#endif /* JAFG_WITH_EDITOR */

#if JAFG_WITH_EDITOR
    void LoadDifferentMeshForAllClients(LPath Path, EStaticMeshState State = EStaticMeshStateBits::None)
    {
        this->Path = std::move(Path);
        this->FreeFromHost();
        this->FreeFromDevice();
        this->FreeConvex();
        this->Reload(State);
    }
#endif /* JAFG_WITH_EDITOR */

    FORCEINLINE constexpr bool IsOnHost() const noexcept { return !this->HostMeshes.empty(); }
    ENGINE_API EResult LoadToHost(bool bLoadConvex);
    inline void FreeFromHost() noexcept { algo::orphan(&this->HostMeshes); }

    FORCEINLINE constexpr bool HasConvex() const noexcept { return !this->ConvexHull.empty(); }
    //#
    //# Builds the convex hull of all primitives. If the mesh is not on the host, it has to be
    //# loaded from secondary storage again. Use with care and consider loading meshes with
    //# #EStaticMeshStateBits::Convex initially.
    //#
    ENGINE_API void ReloadConvex();
    inline void FreeConvex() noexcept { algo::orphan(&this->ConvexHull); }

    FORCEINLINE constexpr bool IsOnDevice() const noexcept { return !this->DeviceMeshes.empty(); }
    ENGINE_API void LoadToDevice();
    inline void FreeFromDevice() noexcept { this->DeviceMeshes.clear(); }

    ENGINE_API void Render(LActorRenderInfo const& Info, LWorldTrans const& Transform, std::variant<LMaterialInstance const*,LPbrMaterialInstance const*> InstanceVariant) const;
    ENGINE_API void DrawIndexed(LRenderInfo const& Info) const;

    NODISCARD FORCEINLINE constexpr LWorldAabb3 const& GetAabb() const noexcept { return this->Aabb; }
    //# Vertices of the convex hull in mesh space. Empty if not built, @see #HasConvex.
    NODISCARD FORCEINLINE constexpr TArray<LVec3F> const& GetConvexHull() const noexcept { return this->ConvexHull; }
    NODISCARD FORCEINLINE constexpr LPath const& GetPath() const noexcept { return this->Path; }

    //#
    //# The limit on how many vertices the convex hull can have at most.
    //# Meshes will get simplified if they exceed this limit.
    //#
    static constexpr int ConvexVertexLimit{ 256 };
    //#
    //# The limit on how many vertices this mesh's convex hull can have at most.
    //# INT_MAX means no limit.
    //#
    int MaxConvexVertices{ ConvexVertexLimit };
    static constexpr f32 DefaultConvexTolerance{ 1.0e-3f };
    f32 ConvexTolerance{ DefaultConvexTolerance };

    //# Host memory. Modify with care.
    TArray<HostMesh> HostMeshes;
    //# Device memory. Modify with care.
    /* TODO: This sucks. I am sure we can squish this into a single device buffer. */
    TArray<DeviceMesh> DeviceMeshes;

private:

    LWorldAabb3 Aabb{ maths::identity<LWorldAabb3> };
    TArray<LVec3F> ConvexHull;
    LPath Path;
};

template<> ENGINE_API Detail::LNodeFactoryBase GetEditorNode<LStaticMeshRef>(TEditorNodeCreateInfo<LStaticMeshRef> const& Info) noexcept;

struct LStaticMeshCreateInfo final
{
    int MaxConvexVertices{ LStaticMesh::ConvexVertexLimit };
    f32 ConvexTolerance{ LStaticMesh::DefaultConvexTolerance };
};

} /* ~Namespace Jafg */

template<>
struct std::hash<Jafg::LStaticMesh::Vertex>
{
    FORCEINLINE size_t operator()(Jafg::LStaticMesh::Vertex const& Vertex) const noexcept
    {
        return (hash<glm::vec3>()(Vertex.Position) >> 1) ^ (hash<glm::vec2>()(Vertex.TexCoord) << 1);
    }
};
