// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

//#
//# The transform of a joint relative to its parent joint (or to the space of the skeleton for roots). Kept as
//# translation, rotation and scale, so that poses can be blended. Single precision: Skeletons are small.
//#
struct LJointTransform final
{
    LVec3F T{ 0.0f };
    LQuatF R{ maths::identity<LQuatF> };
    LVec3F S{ 1.0f };

    NODISCARD ENGINE_API LMat4F ToMatrix() const noexcept;

    //#
    //# Linear interpolation of translation and scale; normalized linear interpolation of the rotation along the
    //# shortest path. Good enough for blending poses, which are close to each other.
    //#
    NODISCARD ENGINE_API static LJointTransform Blend(LJointTransform const& From, LJointTransform const& To, f32 Alpha) noexcept;

    NODISCARD bool operator==(LJointTransform const&) const = default;
};

//#
//# Changes the proportions of one joint on top of every pose (e.g., the longer arms of a customized character). All
//# ones change nothing. @see #LSkeleton::ApplyProportions, #LSkeleton::ModelToSkin
//#
struct LJointProportion final
{
    //#
    //# Multiplies the scale of the joint. Like every scale of a joint, it carries over to the joints below it (their
    //# translations and the vertices they move). Keep it uniform if the joint has children, they are skewed otherwise.
    //#
    LVec3F Scale{ 1.0f };
    //#
    //# Deforms the vertices that the joint moves with a linear map in the space of the joint (of its bind pose), around
    //# its origin. The joints directly below it move along with the deformed vertices, but are not deformed themselves.
    //# Unlike #Scale, it may be anything. E.g., a scale along the bone (Y in skeletons from Blender) makes the bone
    //# longer and moves the joint at its end, or a scale along the axes of the model makes the body wider.
    //#
    LMat3F Mesh{ 1.0f };

    NODISCARD bool operator==(LJointProportion const&) const = default;
};

//#
//# The joints of a skinned mesh (a glTF skin). Joints are sorted parents first, so that a pose can be resolved from
//# the first to the last joint in one pass.
//#
//# The space of the skeleton (model space) is the space that the skinned mesh is authored in: For a glTF file the
//# space of its scene. Everything above the root joints that is not a joint itself (e.g., the armature object) is
//# folded into #RootParent.
//#
struct LSkeleton final
{
    struct LJoint final
    {
        LString Name;
        //# The index of the parent joint. Always smaller than the index of this joint. Negative for roots.
        i32 Parent{ -1 };
        //# The rest pose of this joint relative to its parent.
        LJointTransform Rest;
        //# From model space to the space of this joint in the bind pose.
        LMat4F InverseBind{ maths::identity<LMat4F> };
    };

    TArray<LJoint> Joints;
    //# The transform of everything above the root joints (in model space). Applied to the roots.
    LMat4F RootParent{ maths::identity<LMat4F> };

    NODISCARD FORCEINLINE std::size_t GetJointCount() const noexcept { return this->Joints.size(); }
    //# The first joint with this name. Empty if there is none. Names are unique in skeletons built by the engine.
    NODISCARD ENGINE_API std::optional<u32> FindJoint(LStringView Name) const noexcept;

    //# Rebuilds the name lookup. Call after the joints were changed.
    ENGINE_API void RebuildLookup();
    //# Whether the joints are sorted parents first and every parent is in range.
    NODISCARD ENGINE_API bool IsValid() const noexcept;

    //# @param Out Resized to the joint count.
    ENGINE_API void GetRestPose(TArray<LJointTransform>& Out) const;
    //# Resolves a local pose into model space. @param OutModel Must have the size of #Local.
    ENGINE_API void LocalToModel(std::span<LJointTransform const> Local, std::span<LMat4F> OutModel) const noexcept;
    //#
    //# The skinning matrices of a model space pose: from the bind pose to the posed model space.
    //# @param OutSkin Must have the size of #Model.
    //#
    ENGINE_API void ModelToSkin(std::span<LMat4F const> Model, std::span<LMat4F> OutSkin) const noexcept;
    //#
    //# Like #ModelToSkin, but the vertices of every joint are additionally deformed by its #LJointProportion::Mesh.
    //# @param Proportions One per joint, or empty for none.
    //#
    ENGINE_API void ModelToSkin(std::span<LMat4F const> Model, std::span<LMat4F> OutSkin, std::span<LJointProportion const> Proportions) const noexcept;
    //#
    //# Applies #LJointProportion::Scale to a local pose, and moves every joint along with the #LJointProportion::Mesh
    //# of its parent. Before the pose is resolved with #LocalToModel.
    //# @param Proportions One per joint, or empty for none.
    //#
    ENGINE_API void ApplyProportions(std::span<LJointTransform> Local, std::span<LJointProportion const> Proportions) const noexcept;

    //# The rotation of a joint in model space, resolved from a local pose. Scale is ignored.
    NODISCARD ENGINE_API LQuatF GetModelRotation(std::span<LJointTransform const> Local, u32 Joint) const noexcept;
    //#
    //# Rotates a joint (and with it everything below it) around its own origin by a rotation that is given in
    //# model space. E.g., to let the head follow the pitch of the view.
    //#
    ENGINE_API void RotateInModelSpace(std::span<LJointTransform> Local, u32 Joint, LQuatF const& ModelRotation) const noexcept;

private:

    std::unordered_map<LString, u32> Lookup;
};

} /* ~Namespace Jafg */
