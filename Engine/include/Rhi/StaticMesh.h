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
typedef rhi::shared_ref<LPbrMaterialInstance> LPbrMaterialInstanceRef;

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

//# Where the data of a static mesh lives in secondary storage.
struct LStaticMeshSource final
{
    //# The namespace the file lives in.
    LPluginInfo Namespace{ nullptr };
    //# A .gltf or .glb file relative to the content dir of the #Namespace. Lexically normalized by #JMeshSubsystem.
    LString Path;
    //# The index of the glTF mesh in the file.
    u32 MeshIdx{};

    NODISCARD bool operator==(LStaticMeshSource const&) const = default;
};

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
        NoSuchMesh,
        NoSuchScene,
    };

    //# Pulls the host data of this mesh. @see #BringToState
    typedef TFunction<EResult(TArray<HostMesh>& Out)> LPullHost;

    inline LStaticMesh() = default;
    //# @param InAbsolutePath The file of the source. Read whenever host data has to be pulled from the source.
    inline explicit LStaticMesh(LStaticMeshSource InSource, LPath InAbsolutePath)
        : Source{std::move(InSource)}, AbsolutePath{std::move(InAbsolutePath)} {}
    PROHIBIT_REALLOC_OF_ANY_FORM(LStaticMesh)
    ~LStaticMesh() = default;

    //# Brings the mesh to at least this state. If already in this state this is a noop.
    ENGINE_API EResult BringToState(EStaticMeshState State);
    //#
    //# Same as above, but if host data is required, it is pulled with the given function instead of reading the
    //# source file. Used to load many meshes of one file while parsing the file only once.
    //# @see #JMeshSubsystem::LoadScene
    //#
    ENGINE_API EResult BringToState(EStaticMeshState State, LPullHost const& PullHost);

#if JAFG_WITH_EDITOR
    //# Reload, due to corruption, etc. Usually not used outside the editor.
    void Reload(EStaticMeshState State);
#endif /* JAFG_WITH_EDITOR */

    FORCEINLINE constexpr bool IsOnHost() const noexcept { return !this->HostMeshes.empty(); }
    //#
    //# Reads the source file and loads only this mesh to the host. Replaces the current host data on success.
    //# Prefer #JMeshSubsystem::LoadScene for files with many meshes. The file is then read only once.
    //#
    ENGINE_API EResult LoadToHost(bool bLoadConvex);
    //# Same as above, but pulls the host data with the given function.
    ENGINE_API EResult LoadToHost(bool bLoadConvex, LPullHost const& PullHost);
    inline void FreeFromHost() noexcept { algo::orphan(&this->HostMeshes); }

    FORCEINLINE constexpr bool HasConvex() const noexcept { return !this->ConvexHull.empty(); }
    //#
    //# Builds the convex hull of all primitives. If the mesh is not on the host, it has to be
    //# loaded from secondary storage again. Use with care and consider loading meshes with
    //# #EStaticMeshStateBits::Convex initially.
    //#
    ENGINE_API void ReloadConvex();
    inline void FreeConvex() noexcept { algo::orphan(&this->ConvexHull); }

    //#
    //# Editor builds only (empty otherwise). The triangles of all primitives in mesh space. Built whenever the mesh
    //# is loaded to the host and kept when the host memory is freed. So that the mesh can be traced precisely (e.g.,
    //# when clicking in the editor) without keeping all vertex data.
    //#
    FORCEINLINE constexpr bool HasTraceGeometry() const noexcept { return !this->TraceIndices.empty(); }
    inline void FreeTraceGeometry() noexcept { algo::orphan(&this->TracePositions); algo::orphan(&this->TraceIndices); }
    //#
    //# The closest hit of the ray with the trace geometry. Both faces of the triangles are hit.
    //# @param Origin In mesh space.
    //# @param Direction In mesh space. Need not be normalized.
    //# @return The parameter along the ray (in multiples of #Direction) in [0, MaxParameter]. Empty if missed or if
    //#         there is no trace geometry (@see #HasTraceGeometry).
    //#
    NODISCARD ENGINE_API std::optional<f32> TraceTriangles(LVec3F const& Origin, LVec3F const& Direction, f32 MaxParameter) const noexcept;

    FORCEINLINE constexpr bool IsOnDevice() const noexcept { return !this->DeviceMeshes.empty(); }
    ENGINE_API void LoadToDevice();
    inline void FreeFromDevice() noexcept { this->DeviceMeshes.clear(); }

    ENGINE_API void Render(LActorRenderInfo const& Info, LWorldTrans const& Transform, std::variant<LMaterialInstance const*,LPbrMaterialInstance const*> InstanceVariant) const;
    //#
    //# Draws with the material instance. The pbr material (or #rhi::invalid_resource_index) is the bindless index
    //# that is pushed with it, e.g. the one the mesh is drawn with otherwise if the instance is a preferred one.
    //#
    ENGINE_API void Render(LActorRenderInfo const& Info, LWorldTrans const& Transform, LMaterialInstance const& Instance, rhi::resource_index_t PbrMaterial) const;
    ENGINE_API void DrawIndexed(LRenderInfo const& Info) const;

    NODISCARD FORCEINLINE constexpr LWorldAabb3 const& GetAabb() const noexcept { return this->Aabb; }
    //# Vertices of the convex hull in mesh space. Empty if not built, @see #HasConvex.
    NODISCARD FORCEINLINE constexpr TArray<LVec3F> const& GetConvexHull() const noexcept { return this->ConvexHull; }
    //# The absolute path to the file of the source.
    NODISCARD FORCEINLINE constexpr LPath const& GetPath() const noexcept { return this->AbsolutePath; }
    NODISCARD FORCEINLINE constexpr LStaticMeshSource const& GetSource() const noexcept { return this->Source; }

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

    //# Reads the source file and pulls the host data of this mesh only.
    NODISCARD EResult PullHostFromSource(TArray<HostMesh>& Out) const;
    //# (Re)builds the trace geometry from the host data. @see #HasTraceGeometry
    void BuildTraceGeometry();

    LWorldAabb3 Aabb{ maths::identity<LWorldAabb3> };
    TArray<LVec3F> ConvexHull;
    //# @see #HasTraceGeometry. Triangle lists.
    TArray<LVec3F> TracePositions;
    TArray<u32> TraceIndices;
    LStaticMeshSource Source;
    //# Resolved once when allocated. So the namespace of the source never has to be dereferenced again.
    LPath AbsolutePath;
};

//#
//# The material instance that a mesh of an actor is drawn with: The one the draw call prefers (e.g., the material of
//# the shadow pass), else the one the editor prefers, else #Owned. Fatal if there is none.
//#
NODISCARD ENGINE_API LMaterialInstance const& GetPreferredMaterialInstance(LActorRenderInfo const& Info, LMaterialInstance const* Owned) noexcept;
//#
//# Binds the pipeline, the descriptor sets and the push constants of a material instance to draw a mesh of an actor
//# with #Transform. Everything that draws vertices of #LStaticMesh::Vertex for an actor goes through this.
//#
ENGINE_API void BindActorMeshMaterial(LActorRenderInfo const& Info, LWorldTrans const& Transform, std::variant<LMaterialInstance const*,LPbrMaterialInstance const*> InstanceVariant);
//# The pbr material (or #rhi::invalid_resource_index) is the bindless index that is pushed with the material instance.
ENGINE_API void BindActorMeshMaterial(LActorRenderInfo const& Info, LWorldTrans const& Transform, LMaterialInstance const& Instance, rhi::resource_index_t PbrMaterial);
//#
//# Draws a mesh of an actor with its own pbr material: Right away with the planned materials (@see
//# LPbrMaterialInstance::GetDrawPlan) if it is opaque, else queued into #LActorRenderInfo::TranslucentDraws if the
//# draw defers them. #Draw has to bind the given instance (with the bindless index of the pbr material) and draw the
//# mesh. It might be called later in the same draw of the world, so it must not refer to anything that does not live
//# until then. #Center is the center of the bounds of the mesh in the world, by which the translucent draws are sorted.
//#
ENGINE_API void DrawWithPbrMaterial(LActorRenderInfo const& Info, LPbrMaterialInstanceRef const& Pbr, LWorldVec3 const& Center
    , TFunction2<void(LActorRenderInfo const& Info, LMaterialInstance const& Instance)> Draw);
//# The center of the bounds in mesh space in the world.
NODISCARD ENGINE_API LWorldVec3 GetWorldCenter(LWorldAabb3 const& Aabb, LWorldTrans const& Transform) noexcept;

template<> ENGINE_API Detail::LNodeFactoryBase GetEditorNode<LStaticMeshRef>(TEditorNodeCreateInfo<LStaticMeshRef> const& Info) noexcept;

struct LStaticMeshCreateInfo final
{
    int MaxConvexVertices{ LStaticMesh::ConvexVertexLimit };
    f32 ConvexTolerance{ LStaticMesh::DefaultConvexTolerance };
};

} /* ~Namespace Jafg */

template<>
struct std::hash<Jafg::LStaticMeshSource>
{
    FORCEINLINE size_t operator()(Jafg::LStaticMeshSource const& Source) const noexcept
    {
        return hash<Jafg::LPluginInfo>()(Source.Namespace) ^ (hash<LString>()(Source.Path) << 1) ^ (hash<u32>()(Source.MeshIdx) << 2);
    }
};

//# "Path#MeshIdx".
template<>
struct std::formatter<Jafg::LStaticMeshSource>: std::formatter<LString>
{
    auto format(Jafg::LStaticMeshSource const& Source, std::format_context& Context) const
    {
        return std::formatter<LString>::format(std::format("{}#{}", Source.Path, Source.MeshIdx), Context);
    }
};

template<>
struct std::hash<Jafg::LStaticMesh::Vertex>
{
    FORCEINLINE size_t operator()(Jafg::LStaticMesh::Vertex const& Vertex) const noexcept
    {
        return (hash<glm::vec3>()(Vertex.Position) >> 1) ^ (hash<glm::vec2>()(Vertex.TexCoord) << 1);
    }
};
