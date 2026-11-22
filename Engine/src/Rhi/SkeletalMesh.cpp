// Copyright mzoesch. All rights reserved.

#include "Rhi/SkeletalMesh.h"
#include "Engine/Engine.h"

namespace
{

NODISCARD rhi::device_buffer StageIndices(TArray<u32> const& Indices)
{
    check(!Indices.empty())
    return Jafg::Detail::GMutableEngine->GetLocalEgo().GetFrontend().Vk_StageBuffer(Jafg::LStageBufferCreateInfo::Index({
        .BufferCopy = vk::BufferCopy{0, 0, sizeof(u32) * Indices.size()},
        .Data = Indices.data(),
        }));
}

} /* ~Namespace <Anonymous> */

void Jafg::LSkeletalMesh::Finalize()
{
    check(this->Skeleton)

    this->VertexCount = 0;
    this->BindAabb = maths::identity<LWorldAabb3>;
    this->JointRadii.assign(this->Skeleton->GetJointCount(), 0.0f);

    /* Where the joints are in the bind pose. */
    TArray<LVec3F> JointOrigins(this->Skeleton->GetJointCount());
    for (auto Idx{0uz}; Idx < JointOrigins.size(); ++Idx)
    {
        JointOrigins[Idx] = LVec3F{glm::inverse(this->Skeleton->Joints[Idx].InverseBind)[3]};
    }

    for (auto& Primitive: this->Primitives)
    {
        check(Primitive.Vertices.size() == Primitive.Influences.size())

        Primitive.FirstVertex = this->VertexCount;
        this->VertexCount += static_cast<u32>(Primitive.Vertices.size());

        for (auto Idx{0uz}; Idx < Primitive.Vertices.size(); ++Idx)
        {
            LVec3F const& Position{Primitive.Vertices[Idx].Position};
            this->BindAabb.inline_merge(LWorldAabb3{.min = LWorldVec3{Position}, .max = LWorldVec3{Position}});

            LSkinInfluence const& Influence{Primitive.Influences[Idx]};
            for (auto Slot{0uz}; Slot < Influence.Joints.size(); ++Slot)
            {
                if (Influence.Weights[Slot] > 0.0f)
                {
                    u16 const Joint{Influence.Joints[Slot]};
                    check(Joint < JointOrigins.size())
                    this->JointRadii[Joint] = maths::max(this->JointRadii[Joint], glm::distance(Position, JointOrigins[Joint]));
                }
            }
        }
    }
}

void Jafg::LSkeletalMesh::LoadToDevice()
{
    LOG_TRACE(LogRhi, "[{}]: Loading skeletal mesh to device.", this->Path)

    for (auto& Primitive: this->Primitives)
    {
        Primitive.DeviceIndices = ::StageIndices(Primitive.Indices);
    }
}

Jafg::LSkeletalMesh::LSubset const& Jafg::LSkeletalMesh::GetSubset(std::vector<bool> const& Mask, f32 MinWeight /* = 0.5f */)
{
    check(this->Skeleton)
    check(Mask.size() == this->Skeleton->GetJointCount())

    auto [It, bInserted]{this->Subsets.try_emplace(std::pair{Mask, MinWeight})};
    if (!bInserted)
    {
        return It->second;
    }

    LSubset& Subset{It->second};
    Subset.Parts.resize(this->Primitives.size());
    for (auto PrimitiveIdx{0uz}; PrimitiveIdx < this->Primitives.size(); ++PrimitiveIdx)
    {
        LPrimitive const& Primitive{this->Primitives[PrimitiveIdx]};

        TArray<bool> Inside(Primitive.Vertices.size());
        for (auto Idx{0uz}; Idx < Primitive.Influences.size(); ++Idx)
        {
            f32 Weight{};
            LSkinInfluence const& Influence{Primitive.Influences[Idx]};
            for (auto Slot{0uz}; Slot < Influence.Joints.size(); ++Slot)
            {
                if (Mask[Influence.Joints[Slot]])
                {
                    Weight += Influence.Weights[Slot];
                }
            }
            Inside[Idx] = Weight >= MinWeight;
        }

        TArray<u32> Indices;
        for (auto Idx{0uz}; Idx + 2 < Primitive.Indices.size(); Idx += 3)
        {
            u32 const A{Primitive.Indices[Idx + 0]};
            u32 const B{Primitive.Indices[Idx + 1]};
            u32 const C{Primitive.Indices[Idx + 2]};
            if (Inside[A] && Inside[B] && Inside[C])
            {
                Indices.insert(Indices.end(), {A, B, C});
            }
        }

        if (!Indices.empty())
        {
            Subset.Parts[PrimitiveIdx].Indices = ::StageIndices(Indices);
            Subset.Parts[PrimitiveIdx].IndexCount = static_cast<u32>(Indices.size());
            Subset.TriangleCount += static_cast<u32>(Indices.size() / 3uz);
        }
    }

    LOG_VERBOSE(LogRhi, "[{}]: Built a subset of [{}] triangles for [{}] joints."
        , this->Path, Subset.TriangleCount, std::ranges::count(Mask, true))
    return Subset;
}
