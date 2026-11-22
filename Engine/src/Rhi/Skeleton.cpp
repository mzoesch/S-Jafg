// Copyright mzoesch. All rights reserved.

#include "Rhi/Skeleton.h"

LMat4F Jafg::LJointTransform::ToMatrix() const noexcept
{
    return glm::translate(maths::identity<LMat4F>, this->T)
        *  glm::mat4_cast(this->R)
        *  glm::scale(maths::identity<LMat4F>, this->S);
}

Jafg::LJointTransform Jafg::LJointTransform::Blend(LJointTransform const& From, LJointTransform const& To, f32 Alpha) noexcept
{
    if (Alpha <= 0.0f)
    {
        return From;
    }
    if (Alpha >= 1.0f)
    {
        return To;
    }

    /* The shortest path. q and -q are the same rotation. */
    LQuatF const Target{glm::dot(From.R, To.R) < 0.0f ? -To.R : To.R};
    LQuatF Rotation{From.R * (1.0f - Alpha) + Target * Alpha};
    f32 const Length{glm::length(Rotation)};
    Rotation = Length > 1.0e-8f ? Rotation / Length : From.R;

    return LJointTransform{
        .T = glm::mix(From.T, To.T, Alpha),
        .R = Rotation,
        .S = glm::mix(From.S, To.S, Alpha),
        };
}

std::optional<u32> Jafg::LSkeleton::FindJoint(LStringView Name) const noexcept
{
    if (!this->Lookup.empty())
    {
        if (auto const It{this->Lookup.find(LString{Name})}; It != this->Lookup.end())
        {
            return It->second;
        }
        return {};
    }

    /* Not built (e.g., a skeleton that was assembled by hand). */
    for (auto Idx{0uz}; Idx < this->Joints.size(); ++Idx)
    {
        if (this->Joints[Idx].Name == Name)
        {
            return static_cast<u32>(Idx);
        }
    }
    return {};
}

void Jafg::LSkeleton::RebuildLookup()
{
    this->Lookup.clear();
    this->Lookup.reserve(this->Joints.size());
    for (auto Idx{0uz}; Idx < this->Joints.size(); ++Idx)
    {
        /* The first one wins, as with #FindJoint without a lookup. */
        this->Lookup.try_emplace(this->Joints[Idx].Name, static_cast<u32>(Idx));
    }
}

bool Jafg::LSkeleton::IsValid() const noexcept
{
    for (auto Idx{0uz}; Idx < this->Joints.size(); ++Idx)
    {
        if (i32 const Parent{this->Joints[Idx].Parent}; Parent >= 0 && static_cast<std::size_t>(Parent) >= Idx)
        {
            return false;
        }
    }
    return true;
}

void Jafg::LSkeleton::GetRestPose(TArray<LJointTransform>& Out) const
{
    Out.resize(this->Joints.size());
    for (auto Idx{0uz}; Idx < this->Joints.size(); ++Idx)
    {
        Out[Idx] = this->Joints[Idx].Rest;
    }
}

void Jafg::LSkeleton::LocalToModel(std::span<LJointTransform const> Local, std::span<LMat4F> OutModel) const noexcept
{
    check(Local.size() == this->Joints.size())
    check(OutModel.size() == this->Joints.size())

    for (auto Idx{0uz}; Idx < this->Joints.size(); ++Idx)
    {
        i32 const Parent{this->Joints[Idx].Parent};
        LMat4F const& ParentModel{Parent >= 0 ? OutModel[static_cast<std::size_t>(Parent)] : this->RootParent};
        OutModel[Idx] = ParentModel * Local[Idx].ToMatrix();
    }
}

void Jafg::LSkeleton::ModelToSkin(std::span<LMat4F const> Model, std::span<LMat4F> OutSkin) const noexcept
{
    check(Model.size() == this->Joints.size())
    check(OutSkin.size() == this->Joints.size())

    for (auto Idx{0uz}; Idx < this->Joints.size(); ++Idx)
    {
        OutSkin[Idx] = Model[Idx] * this->Joints[Idx].InverseBind;
    }
}

LQuatF Jafg::LSkeleton::GetModelRotation(std::span<LJointTransform const> Local, u32 Joint) const noexcept
{
    check(Local.size() == this->Joints.size())
    check(Joint < this->Joints.size())

    LQuatF Result{Local[Joint].R};
    for (i32 Parent{this->Joints[Joint].Parent}; Parent >= 0; Parent = this->Joints[static_cast<std::size_t>(Parent)].Parent)
    {
        Result = Local[static_cast<std::size_t>(Parent)].R * Result;
    }
    return glm::normalize(glm::quat_cast(LMat3F{this->RootParent}) * Result);
}

void Jafg::LSkeleton::RotateInModelSpace(std::span<LJointTransform> Local, u32 Joint, LQuatF const& ModelRotation) const noexcept
{
    check(Local.size() == this->Joints.size())
    check(Joint < this->Joints.size())

    /*
     * The joint's model rotation M = P * L (P: of the parent). The rotated one is R * M = P * L'. So
     * L' = P^-1 * R * P * L.
     */
    i32 const Parent{this->Joints[Joint].Parent};
    LQuatF const ParentModel{Parent >= 0
        ? this->GetModelRotation(Local, static_cast<u32>(Parent))
        : glm::normalize(glm::quat_cast(LMat3F{this->RootParent}))};
    Local[Joint].R = glm::normalize(glm::inverse(ParentModel) * ModelRotation * ParentModel * Local[Joint].R);
}
