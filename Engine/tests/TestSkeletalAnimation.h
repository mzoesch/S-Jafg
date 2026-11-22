// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/Animation.h"
#include "Rhi/SkeletalMesh.h"

namespace TestSkeletalAnimation
{

inline constexpr f32 Epsilon{ 1.0e-4f };

NODISCARD inline f32 Distance(LVec3F const& A, LVec3F const& B) noexcept
{
    return glm::length(A - B);
}

//# The angle between two rotations in radians.
NODISCARD inline f32 Angle(LQuatF const& A, LQuatF const& B) noexcept
{
    return 2.0f * std::acos(maths::min(1.0f, std::abs(glm::dot(A, B))));
}

NODISCARD inline LQuatF AroundY(f32 Radians) noexcept
{
    return glm::angleAxis(Radians, LVec3F{0.0f, 1.0f, 0.0f});
}

//#
//# A chain of two joints: "Root" at the origin and "Tip" one meter above it. The inverse bind matrices are the ones
//# of the rest pose.
//#
NODISCARD inline Jafg::LSkeleton MakeChain()
{
    Jafg::LSkeleton Skeleton;
    Skeleton.Joints.push_back({.Name = "Root", .Parent = -1});
    Skeleton.Joints.push_back({.Name = "Tip", .Parent = 0, .Rest = {.T = LVec3F{0.0f, 1.0f, 0.0f}}});

    TArray<Jafg::LJointTransform> Rest;
    Skeleton.GetRestPose(Rest);
    TArray<LMat4F> Model(2);
    Skeleton.LocalToModel(Rest, Model);
    for (auto Idx{0uz}; Idx < 2; ++Idx)
    {
        Skeleton.Joints[Idx].InverseBind = glm::inverse(Model[Idx]);
    }
    Skeleton.RebuildLookup();
    return Skeleton;
}

//# A linear clip that moves #Target from #From to #To within one second.
NODISCARD inline Jafg::LAnimationClipRef MakeMove(LStringView Name, LStringView Target, LVec3F const& From, LVec3F const& To)
{
    auto Clip{std::make_shared<Jafg::LAnimationClip>()};
    Clip->Name = Name;
    Clip->Duration = 1.0f;
    Clip->Channels.push_back({
        .Target = LString{Target},
        .Path = Jafg::EAnimationPath::Translation,
        .Interpolation = Jafg::EAnimationInterpolation::Linear,
        .Times = {0.0f, 1.0f},
        .Values = {LVec4F{From, 0.0f}, LVec4F{To, 0.0f}},
        });
    return Clip;
}

} /* ~Namespace TestSkeletalAnimation */

TRIVIAL_TEST_CASE("animation", JointTransforms)
{
    using namespace TestSkeletalAnimation;

    Jafg::LJointTransform const A{.T = LVec3F{0.0f}, .R = AroundY(0.0f), .S = LVec3F{1.0f}};
    Jafg::LJointTransform const B{.T = LVec3F{2.0f, 0.0f, 0.0f}, .R = AroundY(maths::half_pi_f), .S = LVec3F{3.0f}};

    Jafg::LJointTransform const Half{Jafg::LJointTransform::Blend(A, B, 0.5f)};
    TEST_LESS(Distance(Half.T, LVec3F{1.0f, 0.0f, 0.0f}), Epsilon)
    TEST_LESS(Distance(Half.S, LVec3F{2.0f}), Epsilon)
    TEST_LESS(Angle(Half.R, AroundY(maths::half_pi_f * 0.5f)), 1.0e-3f)
    TEST_TRUE(Jafg::LJointTransform::Blend(A, B, 0.0f) == A)
    TEST_TRUE(Jafg::LJointTransform::Blend(A, B, 1.0f) == B)

    /* q and -q are the same rotation. Blending takes the short way. */
    Jafg::LJointTransform Flipped{B};
    Flipped.R = -Flipped.R;
    TEST_LESS(Angle(Jafg::LJointTransform::Blend(A, Flipped, 0.5f).R, AroundY(maths::half_pi_f * 0.5f)), 1.0e-3f)

    /* Translation, then rotation, then scale. */
    LVec3F const Point{LVec3F{B.ToMatrix() * LVec4F{1.0f, 0.0f, 0.0f, 1.0f}}};
    TEST_LESS(Distance(Point, LVec3F{2.0f, 0.0f, -3.0f}), Epsilon)
}

TRIVIAL_TEST_CASE("animation", SkeletonPoses)
{
    using namespace TestSkeletalAnimation;

    Jafg::LSkeleton const Skeleton{MakeChain()};
    TEST_TRUE(Skeleton.IsValid())
    TEST_TRUE(Skeleton.FindJoint("Tip") == std::optional<u32>{1u})
    TEST_FALSE(Skeleton.FindJoint("Nope").has_value())

    TArray<Jafg::LJointTransform> Pose;
    Skeleton.GetRestPose(Pose);
    TArray<LMat4F> Model(2);
    TArray<LMat4F> Skin(2);
    Skeleton.LocalToModel(Pose, Model);
    Skeleton.ModelToSkin(Model, Skin);
    /* The rest pose is the bind pose: Skinning changes nothing. */
    TEST_LESS(Distance(LVec3F{Skin[1] * LVec4F{0.3f, 1.2f, 0.0f, 1.0f}}, LVec3F{0.3f, 1.2f, 0.0f}), Epsilon)

    /* Turning the root turns the tip around it. */
    Pose[0].R = glm::angleAxis(maths::half_pi_f, LVec3F{0.0f, 0.0f, 1.0f});
    Skeleton.LocalToModel(Pose, Model);
    TEST_LESS(Distance(LVec3F{Model[1][3]}, LVec3F{-1.0f, 0.0f, 0.0f}), Epsilon)

    /* A rotation in model space around the joint itself. The tip turns around the root, which stays. */
    Skeleton.GetRestPose(Pose);
    Pose[0].R = AroundY(maths::half_pi_f);
    LQuatF const Tilt{glm::angleAxis(maths::half_pi_f, LVec3F{1.0f, 0.0f, 0.0f})};
    Skeleton.RotateInModelSpace(Pose, 0, Tilt);
    TEST_LESS(Angle(Skeleton.GetModelRotation(Pose, 0), Tilt * AroundY(maths::half_pi_f)), 1.0e-3f)
    Skeleton.LocalToModel(Pose, Model);
    TEST_LESS(Distance(LVec3F{Model[0][3]}, LVec3F{0.0f}), Epsilon)
    TEST_LESS(Distance(LVec3F{Model[1][3]}, LVec3F{0.0f, 0.0f, 1.0f}), Epsilon)
}

TRIVIAL_TEST_CASE("animation", ChannelSampling)
{
    using namespace TestSkeletalAnimation;

    Jafg::LAnimationChannel Linear{
        .Target = "Root",
        .Path = Jafg::EAnimationPath::Translation,
        .Interpolation = Jafg::EAnimationInterpolation::Linear,
        .Times = {1.0f, 2.0f},
        .Values = {LVec4F{0.0f}, LVec4F{4.0f, 0.0f, 0.0f, 0.0f}},
        };
    TEST_LESS(Distance(LVec3F{Linear.Sample(1.5f)}, LVec3F{2.0f, 0.0f, 0.0f}), Epsilon)
    /* Clamped to the first and the last key. */
    TEST_LESS(Distance(LVec3F{Linear.Sample(0.0f)}, LVec3F{0.0f}), Epsilon)
    TEST_LESS(Distance(LVec3F{Linear.Sample(9.0f)}, LVec3F{4.0f, 0.0f, 0.0f}), Epsilon)

    Jafg::LAnimationChannel Step{Linear};
    Step.Interpolation = Jafg::EAnimationInterpolation::Step;
    TEST_LESS(Distance(LVec3F{Step.Sample(1.99f)}, LVec3F{0.0f}), Epsilon)
    TEST_LESS(Distance(LVec3F{Step.Sample(2.0f)}, LVec3F{4.0f, 0.0f, 0.0f}), Epsilon)

    /* Without tangents a cubic spline passes the middle at half of the segment. */
    Jafg::LAnimationChannel Cubic{Linear};
    Cubic.Interpolation = Jafg::EAnimationInterpolation::CubicSpline;
    Cubic.Values = {LVec4F{0.0f}, LVec4F{0.0f}, LVec4F{0.0f}, LVec4F{0.0f}, LVec4F{4.0f, 0.0f, 0.0f, 0.0f}, LVec4F{0.0f}};
    TEST_LESS(Distance(LVec3F{Cubic.Sample(1.5f)}, LVec3F{2.0f, 0.0f, 0.0f}), Epsilon)
    TEST_LESS(Distance(LVec3F{Cubic.Sample(1.25f)}, LVec3F{4.0f * 0.15625f, 0.0f, 0.0f}), Epsilon)

    /* Rotations are interpolated spherically. */
    LQuatF const To{AroundY(maths::half_pi_f)};
    Jafg::LAnimationChannel Rotation{
        .Target = "Root",
        .Path = Jafg::EAnimationPath::Rotation,
        .Interpolation = Jafg::EAnimationInterpolation::Linear,
        .Times = {0.0f, 1.0f},
        .Values = {LVec4F{0.0f, 0.0f, 0.0f, 1.0f}, LVec4F{To.x, To.y, To.z, To.w}},
        };
    LVec4F const Half{Rotation.Sample(0.5f)};
    TEST_LESS(Angle(LQuatF::wxyz(Half.w, Half.x, Half.y, Half.z), AroundY(maths::half_pi_f * 0.5f)), 1.0e-3f)
}

TRIVIAL_TEST_CASE("animation", ClipsAndPlayer)
{
    using namespace TestSkeletalAnimation;

    Jafg::LSkeleton const Skeleton{MakeChain()};
    auto const Up{MakeMove("Up", "Tip", LVec3F{0.0f, 1.0f, 0.0f}, LVec3F{0.0f, 3.0f, 0.0f})};
    auto const Side{MakeMove("Side", "Tip", LVec3F{2.0f, 1.0f, 0.0f}, LVec3F{2.0f, 1.0f, 0.0f})};

    /* Channels bind by the name of the joint. Unknown joints are ignored. */
    auto Other{std::make_shared<Jafg::LAnimationClip>(*Up)};
    Other->Channels.front().Target = "Elsewhere";
    TEST_EQUAL(Jafg::LAnimationClipBinding::Bind(*Up, Skeleton).Entries.size(), 1uz)
    TEST_EQUAL(Jafg::LAnimationClipBinding::Bind(*Other, Skeleton).UnboundChannels, 1u)

    std::unordered_map<Jafg::LAnimationClip const*, Jafg::LAnimationClipBinding> Bindings;
    auto const Resolve{[&](Jafg::LAnimationClip const& Clip) -> Jafg::LAnimationClipBinding const&
    {
        auto [It, bInserted]{Bindings.try_emplace(&Clip)};
        if (bInserted)
        {
            It->second = Jafg::LAnimationClipBinding::Bind(Clip, Skeleton);
        }
        return It->second;
    }};

    Jafg::LAnimationPlayer Player;
    TArray<Jafg::LJointTransform> Pose;

    /* Nothing plays: The rest pose. */
    Player.Evaluate(Skeleton, Resolve, Pose);
    TEST_LESS(Distance(Pose[1].T, LVec3F{0.0f, 1.0f, 0.0f}), Epsilon)

    Player.Play(Up, {.BlendTime = 0.0f});
    Player.Advance(0.5f);
    Player.Evaluate(Skeleton, Resolve, Pose);
    TEST_LESS(Distance(Pose[1].T, LVec3F{0.0f, 2.0f, 0.0f}), Epsilon)

    /* Looping wraps around. */
    Player.Advance(0.75f);
    TEST_LESS(std::abs(Player.GetCurrentTime() - 0.25f), Epsilon)

    /* A crossfade over one second. Halfway, both count the same. */
    Player.Play(Side, {.BlendTime = 1.0f});
    Player.Advance(0.5f);
    Player.Evaluate(Skeleton, Resolve, Pose);
    LVec3F const UpThen{0.0f, 1.0f + 2.0f * 0.75f, 0.0f};
    TEST_LESS(Distance(Pose[1].T, glm::mix(UpThen, LVec3F{2.0f, 1.0f, 0.0f}, 0.5f)), Epsilon)
    TEST_TRUE(Player.GetCurrentClip() == Side.get())

    /* Done fading: Only the new clip is left. */
    Player.Advance(0.5f);
    Player.Evaluate(Skeleton, Resolve, Pose);
    TEST_LESS(Distance(Pose[1].T, LVec3F{2.0f, 1.0f, 0.0f}), Epsilon)

    /* Playing the current clip again only changes its rate. */
    Player.Play(Side, {.Rate = 2.0f});
    TEST_TRUE(Player.GetCurrentClip() == Side.get())

    /* A clip that does not loop stops at its end. */
    Player.Play(Up, {.BlendTime = 0.0f, .bLoop = false});
    TEST_FALSE(Player.HasCurrentClipFinished())
    Player.Advance(5.0f);
    TEST_TRUE(Player.HasCurrentClipFinished())
    Player.Evaluate(Skeleton, Resolve, Pose);
    TEST_LESS(Distance(Pose[1].T, LVec3F{0.0f, 3.0f, 0.0f}), Epsilon)

    /* Stopping fades back to the rest pose. */
    Player.Stop(0.0f);
    Player.Evaluate(Skeleton, Resolve, Pose);
    TEST_LESS(Distance(Pose[1].T, LVec3F{0.0f, 1.0f, 0.0f}), Epsilon)
}

TRIVIAL_TEST_CASE("animation", HostSkinning)
{
    using namespace TestSkeletalAnimation;

    Jafg::LStaticMesh::Vertex Bind{};
    Bind.Position = LVec3F{0.0f, 1.0f, 0.0f};
    Bind.Normal = glm::normalize(LVec3F{1.0f, 1.0f, 0.0f});
    Bind.Tangent = LVec4F{1.0f, 0.0f, 0.0f, -1.0f};

    /* Half and half between a joint that stays and one that moves two meters along x. */
    TArray<LMat4F> const Skin{maths::identity<LMat4F>, glm::translate(maths::identity<LMat4F>, LVec3F{2.0f, 0.0f, 0.0f})};
    Jafg::LSkinInfluence const Half{.Joints = {0, 1, 0, 0}, .Weights = {0.5f, 0.5f, 0.0f, 0.0f}};
    Jafg::LStaticMesh::Vertex Out{};
    Jafg::SkinVertices(std::span{&Bind, 1}, std::span{&Half, 1}, Skin, &Out);
    TEST_LESS(Distance(Out.Position, LVec3F{1.0f, 1.0f, 0.0f}), Epsilon)
    TEST_LESS(Distance(Out.Normal, Bind.Normal), Epsilon)
    TEST_TRUE(Out.Tangent.w == -1.0f)

    /* Normals stay perpendicular to the surface under non-uniform scale (the inverse transpose, not the matrix). */
    TArray<LMat4F> const Stretch{glm::scale(maths::identity<LMat4F>, LVec3F{2.0f, 1.0f, 1.0f})};
    Jafg::LSkinInfluence const Single{.Joints = {0, 0, 0, 0}, .Weights = {1.0f, 0.0f, 0.0f, 0.0f}};
    Jafg::SkinVertices(std::span{&Bind, 1}, std::span{&Single, 1}, Stretch, &Out);
    TEST_LESS(Distance(Out.Normal, glm::normalize(LVec3F{0.5f, 1.0f, 0.0f})), Epsilon)
    TEST_LESS(Distance(LVec3F{Out.Tangent}, LVec3F{1.0f, 0.0f, 0.0f}), Epsilon)

    /* The posed bounds of a mesh contain its skinned vertices. */
    Jafg::LSkeletalMesh Mesh;
    Mesh.Skeleton = std::make_shared<Jafg::LSkeleton>(MakeChain());
    auto& Primitive{Mesh.Primitives.emplace_back()};
    for (f32 const Y: {0.0f, 0.5f, 1.0f, 1.5f})
    {
        Jafg::LStaticMesh::Vertex V{};
        V.Position = LVec3F{0.25f, Y, 0.0f};
        Primitive.Vertices.push_back(V);
        Primitive.Influences.push_back({.Joints = {0, 1, 0, 0}, .Weights = {1.0f - Y / 1.5f, Y / 1.5f, 0.0f, 0.0f}});
    }
    Mesh.Finalize();
    TEST_EQUAL(Mesh.VertexCount, 4u)
    /* The tip (at y = 1) moves the vertices at y = 0.5, 1 and 1.5. */
    TEST_LESS(std::abs(Mesh.JointRadii[1] - glm::length(LVec3F{0.25f, 0.5f, 0.0f})), Epsilon)
    /* The root (at the origin) moves the vertices at y = 0, 0.5 and 1. */
    TEST_LESS(std::abs(Mesh.JointRadii[0] - glm::length(LVec3F{0.25f, 1.0f, 0.0f})), Epsilon)

    TArray<Jafg::LJointTransform> Pose;
    Mesh.Skeleton->GetRestPose(Pose);
    Pose[0].R = glm::angleAxis(1.0f, LVec3F{0.0f, 0.0f, 1.0f});
    Pose[1].R = glm::angleAxis(-0.7f, LVec3F{1.0f, 0.0f, 0.0f});
    TArray<LMat4F> Model(2);
    TArray<LMat4F> Posed(2);
    Mesh.Skeleton->LocalToModel(Pose, Model);
    Mesh.Skeleton->ModelToSkin(Model, Posed);
    TArray<Jafg::LStaticMesh::Vertex> Skinned(Primitive.Vertices.size());
    Jafg::SkinVertices(Primitive.Vertices, Primitive.Influences, Posed, Skinned.data());

    LWorldAabb3 const Bounds{Mesh.ComputePosedAabb(Model)};
    for (auto const& V: Skinned)
    {
        TEST_TRUE(V.Position.x >= Bounds.min.x && V.Position.y >= Bounds.min.y && V.Position.z >= Bounds.min.z)
        TEST_TRUE(V.Position.x <= Bounds.max.x && V.Position.y <= Bounds.max.y && V.Position.z <= Bounds.max.z)
    }
}
