// Copyright mzoesch. All rights reserved.

//#
//# The host skinning kernel (and the bounds of posed meshes). It runs for every vertex of every animated mesh every
//# frame, so it is compiled with optimizations in every configuration (see Engine/CMakeLists.txt): Unoptimized, every
//# vector and matrix operation is a function call and a few thousand vertices already take milliseconds. The loop
//# works on plain floats, so that it is reasonably fast even if it is not optimized (e.g., with another compiler).
//#

#include "Rhi/SkeletalMesh.h"

namespace
{

//# A skinning matrix without its last row (always 0, 0, 0, 1). Row-major: Each row is a dot product with (x, y, z, 1).
struct LRows final
{
    std::array<f32, 12> M;
};

//# More joints than this are put on the heap.
constexpr std::size_t MaxStackJoints{ 256 };

//# The longest axis that a transform stretches a vector by. Exact for transforms without shear, slightly enlarged for the rest.
NODISCARD FORCEINLINE f32 GetMaxStretch(LMat4F const& M) noexcept
{
    constexpr f32 ShearMargin{ 1.1f };
    return maths::max(glm::length(LVec3F{M[0]}), maths::max(glm::length(LVec3F{M[1]}), glm::length(LVec3F{M[2]}))) * ShearMargin;
}

} /* ~Namespace <Anonymous> */

void Jafg::SkinVertices(
      std::span<LStaticMesh::Vertex const> Bind
    , std::span<LSkinInfluence const> Influences
    , std::span<LMat4F const> Skin
    , LStaticMesh::Vertex* Out
    ) noexcept
{
    check(Bind.size() == Influences.size())
    check(Out || Bind.empty())

    /* glm is column-major. */
    std::array<LRows, MaxStackJoints> StackRows;
    std::vector<LRows> HeapRows;
    LRows* Rows{StackRows.data()};
    if (Skin.size() > MaxStackJoints)
    {
        HeapRows.resize(Skin.size());
        Rows = HeapRows.data();
    }
    for (std::size_t Joint{0}; Joint < Skin.size(); ++Joint)
    {
        LMat4F const& S{Skin[Joint]};
        for (glm::length_t Row{0}; Row < 3; ++Row)
        {
            for (glm::length_t Column{0}; Column < 4; ++Column)
            {
                Rows[Joint].M[static_cast<std::size_t>(Row * 4 + Column)] = S[Column][Row];
            }
        }
    }

    LStaticMesh::Vertex const* const In{Bind.data()};
    LSkinInfluence const* const Weights{Influences.data()};
    std::size_t const Count{Bind.size()};

    for (std::size_t Idx{0}; Idx < Count; ++Idx)
    {
        LStaticMesh::Vertex const& V{In[Idx]};
        LSkinInfluence const& W{Weights[Idx]};

        /* The weighted sum of the matrices of the joints. Weights of unused slots are zero. */
        f32 m[12];
        {
            f32 const* const R{Rows[W.Joints[0]].M.data()};
            f32 const w{W.Weights[0]};
            for (std::size_t k{0}; k < 12; ++k)
            {
                m[k] = R[k] * w;
            }
        }
        for (std::size_t Slot{1}; Slot < 4; ++Slot)
        {
            f32 const w{W.Weights[Slot]};
            if (w != 0.0f)
            {
                f32 const* const R{Rows[W.Joints[Slot]].M.data()};
                for (std::size_t k{0}; k < 12; ++k)
                {
                    m[k] += R[k] * w;
                }
            }
        }

        f32 const px{V.Position.x};
        f32 const py{V.Position.y};
        f32 const pz{V.Position.z};

        /*
         * Normals go through the cofactor matrix: Its columns are the cross products of the columns of the linear part,
         * which is the inverse transpose up to the determinant. Its sign is kept, so that mirroring flips the normal.
         */
        f32 const ax{m[5] * m[10] - m[9] * m[6]};
        f32 const ay{m[9] * m[2] - m[1] * m[10]};
        f32 const az{m[1] * m[6] - m[5] * m[2]};
        f32 const bx{m[6] * m[8] - m[10] * m[4]};
        f32 const by{m[10] * m[0] - m[2] * m[8]};
        f32 const bz{m[2] * m[4] - m[6] * m[0]};
        f32 const cx{m[4] * m[9] - m[8] * m[5]};
        f32 const cy{m[8] * m[1] - m[0] * m[9]};
        f32 const cz{m[0] * m[5] - m[4] * m[1]};
        f32 const Sign{m[0] * ax + m[4] * ay + m[8] * az < 0.0f ? -1.0f : 1.0f};

        f32 const nx{V.Normal.x};
        f32 const ny{V.Normal.y};
        f32 const nz{V.Normal.z};
        f32 Nx{(ax * nx + bx * ny + cx * nz) * Sign};
        f32 Ny{(ay * nx + by * ny + cy * nz) * Sign};
        f32 Nz{(az * nx + bz * ny + cz * nz) * Sign};
        if (f32 const Length2{Nx * Nx + Ny * Ny + Nz * Nz}; Length2 > 1.0e-24f)
        {
            f32 const Inverse{1.0f / std::sqrt(Length2)};
            Nx *= Inverse;
            Ny *= Inverse;
            Nz *= Inverse;
        }
        else
        {
            Nx = nx;
            Ny = ny;
            Nz = nz;
        }

        /* Tangents are directions on the surface. */
        f32 const tx{V.Tangent.x};
        f32 const ty{V.Tangent.y};
        f32 const tz{V.Tangent.z};
        f32 Tx{m[0] * tx + m[1] * ty + m[2] * tz};
        f32 Ty{m[4] * tx + m[5] * ty + m[6] * tz};
        f32 Tz{m[8] * tx + m[9] * ty + m[10] * tz};
        if (f32 const Length2{Tx * Tx + Ty * Ty + Tz * Tz}; Length2 > 1.0e-24f)
        {
            f32 const Inverse{1.0f / std::sqrt(Length2)};
            Tx *= Inverse;
            Ty *= Inverse;
            Tz *= Inverse;
        }
        else
        {
            Tx = tx;
            Ty = ty;
            Tz = tz;
        }

        /* One sequential write per vertex: #Out may be write-combined memory. */
        LStaticMesh::Vertex Result;
        Result.Position = LVec3F{
            m[0] * px + m[1] * py + m[2] * pz + m[3],
            m[4] * px + m[5] * py + m[6] * pz + m[7],
            m[8] * px + m[9] * py + m[10] * pz + m[11],
            };
        Result.Normal = LVec3F{Nx, Ny, Nz};
        Result.TexCoord = V.TexCoord;
        Result.Tangent = LVec4F{Tx, Ty, Tz, V.Tangent.w};
        Out[Idx] = Result;
    }
}

LWorldAabb3 Jafg::LSkeletalMesh::ComputePosedAabb(std::span<LMat4F const> Model) const noexcept
{
    check(Model.size() == this->JointRadii.size())

    LWorldAabb3 Result{maths::identity<LWorldAabb3>};
    for (auto Idx{0uz}; Idx < Model.size(); ++Idx)
    {
        if (f32 const Radius{this->JointRadii[Idx]}; Radius > 0.0f)
        {
            /* The skinning matrix of the joint maps its bind origin to its posed origin and stretches like the joint. */
            LMat4F const Skin{Model[Idx] * this->Skeleton->Joints[Idx].InverseBind};
            LVec3F const Center{Model[Idx][3]};
            LVec3F const Extent{Radius * ::GetMaxStretch(Skin)};
            Result.inline_merge(LWorldAabb3{.min = LWorldVec3{Center - Extent}, .max = LWorldVec3{Center + Extent}});
        }
    }
    return Result;
}
