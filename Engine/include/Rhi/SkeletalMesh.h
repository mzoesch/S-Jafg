// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/StaticMesh.h"
#include "Rhi/Skeleton.h"

namespace Jafg
{

//# The joints that move a vertex. Unused slots have a weight of zero. The weights sum up to one.
struct LSkinInfluence final
{
    std::array<u16, 4> Joints{};
    std::array<f32, 4> Weights{};
};

struct LSkeletalMesh;
typedef std::shared_ptr<LSkeletalMesh> LSkeletalMeshRef;

//#
//# A mesh that is deformed by a skeleton (a glTF mesh with a skin). Shared between all components that show it and
//# not changed after it was loaded.
//#
//# The vertices are skinned on the host (#SkinVertices) into vertex buffers that each component owns. The result has
//# the layout of #LStaticMesh::Vertex, so that every material (and every pass, e.g., shadows) that draws static meshes
//# draws skinned meshes as well. Only the indices live on the device here.
//#
//# @see #JMeshSubsystem::LoadSkeletalMesh to load one.
//#
struct LSkeletalMesh final
{
    struct LPrimitive final
    {
        //# In the bind pose. In model space (@see #LSkeleton).
        TArray<LStaticMesh::Vertex> Vertices;
        //# One per vertex. The joints are indices into #Skeleton.
        TArray<LSkinInfluence> Influences;
        TArray<u32> Indices;
        //# Where the vertices of this primitive start in a buffer that holds the vertices of all primitives.
        u32 FirstVertex{};
        rhi::device_buffer DeviceIndices;
    };

    //# The triangles that are mostly moved by a set of joints. @see #GetSubset
    struct LSubset final
    {
        struct LPart final
        {
            //# Null if no triangle of the primitive is part of the subset.
            rhi::device_buffer Indices;
            u32 IndexCount{};
        };
        //# One per primitive.
        TArray<LPart> Parts;
        //# Of all parts.
        u32 TriangleCount{};
    };

    LString Name;
    //# For diagnostics. The file relative to the content dir of its namespace.
    LString Path;
    std::shared_ptr<LSkeleton const> Skeleton;
    TArray<LPrimitive> Primitives;

    //# Of all primitives. Set by #Finalize.
    u32 VertexCount{};
    //# In model space. Set by #Finalize.
    LWorldAabb3 BindAabb{ maths::identity<LWorldAabb3> };
    //#
    //# Per joint: The distance of the farthest vertex that the joint moves from the joint, in the bind pose. Zero if
    //# the joint moves no vertex. Set by #Finalize. Bounds posed meshes without skinning them. @see #ComputePosedAabb
    //#
    TArray<f32> JointRadii;

    //# Computes everything that is derived from the host data. Call once after the primitives and the skeleton were set.
    ENGINE_API void Finalize();

    NODISCARD FORCEINLINE bool IsOnDevice() const noexcept
    {
        return !this->Primitives.empty() && !!*this->Primitives.front().DeviceIndices;
    }
    //# Uploads the indices of all primitives.
    ENGINE_API void LoadToDevice();

    //#
    //# The triangles whose three vertices each get at least #MinWeight of their weight from the joints in #Mask (one
    //# flag per joint). E.g., to draw only the arms of a character in first person. Built on the first request and
    //# cached afterward. Requires the device.
    //#
    ENGINE_API LSubset const& GetSubset(std::vector<bool> const& Mask, f32 MinWeight = 0.5f);

    //#
    //# Bounds of the mesh in a pose, without skinning it. Conservative: Every vertex is near the joints that move it.
    //# @param Model The model space transforms of all joints.
    //#
    NODISCARD ENGINE_API LWorldAabb3 ComputePosedAabb(std::span<LMat4F const> Model) const noexcept;
    //#
    //# Like the other overload, for skinning matrices that do not come from #Model alone (e.g., with the
    //# #LJointProportion::Mesh of #LSkeleton::ModelToSkin).
    //# @param Skin The skinning matrices of all joints.
    //#
    NODISCARD ENGINE_API LWorldAabb3 ComputePosedAabb(std::span<LMat4F const> Model, std::span<LMat4F const> Skin) const noexcept;

private:

    std::map<std::pair<std::vector<bool>, f32>, LSubset> Subsets;
};

//#
//# Linear blend skinning on the host.
//#
//# Normals are transformed by the cofactor matrix of the blended transform, so that they stay perpendicular to the
//# surface even if the joints scale non-uniformly. Tangents are transformed as directions.
//#
//# @param Skin The skinning matrices of the joints (@see #LSkeleton::ModelToSkin).
//# @param Out Gets one vertex per bind vertex. Written strictly sequentially, so that it may point to write-combined
//#            (mapped device) memory.
//#
ENGINE_API void SkinVertices(
      std::span<LStaticMesh::Vertex const> Bind
    , std::span<LSkinInfluence const> Influences
    , std::span<LMat4F const> Skin
    , LStaticMesh::Vertex* Out
    ) noexcept;

} /* ~Namespace Jafg */
