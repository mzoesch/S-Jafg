// Copyright mzoesch. All rights reserved.

#include "Jgc/Framework/JgcHumanoidShapeComponent.h"
#include "Framework/Actor.h"
#include "Framework/SkeletalMeshComponent.h"

namespace
{

constexpr Jgc::LHumanoidShapeParameter Parameters[]{
    {.Group = "Head", .DisplayName = "Size",             .Value = &Jgc::LHumanoidShape::HeadSize,       .Min = 0.70f, .Max = 1.40f},
    {.Group = "Head", .DisplayName = "Width",            .Value = &Jgc::LHumanoidShape::HeadWidth,      .Min = 0.75f, .Max = 1.30f},
    {.Group = "Head", .DisplayName = "Height",           .Value = &Jgc::LHumanoidShape::HeadHeight,     .Min = 0.75f, .Max = 1.30f},
    {.Group = "Head", .DisplayName = "Depth",            .Value = &Jgc::LHumanoidShape::HeadDepth,      .Min = 0.75f, .Max = 1.30f},
    {.Group = "Head", .DisplayName = "Neck length",      .Value = &Jgc::LHumanoidShape::NeckLength,     .Min = 0.50f, .Max = 1.80f},
    {.Group = "Torso", .DisplayName = "Length",          .Value = &Jgc::LHumanoidShape::TorsoLength,    .Min = 0.85f, .Max = 1.20f},
    {.Group = "Torso", .DisplayName = "Chest width",     .Value = &Jgc::LHumanoidShape::ChestWidth,     .Min = 0.80f, .Max = 1.30f},
    {.Group = "Torso", .DisplayName = "Chest depth",     .Value = &Jgc::LHumanoidShape::ChestDepth,     .Min = 0.80f, .Max = 1.30f},
    {.Group = "Torso", .DisplayName = "Waist width",     .Value = &Jgc::LHumanoidShape::WaistWidth,     .Min = 0.80f, .Max = 1.40f},
    {.Group = "Torso", .DisplayName = "Waist depth",     .Value = &Jgc::LHumanoidShape::WaistDepth,     .Min = 0.80f, .Max = 1.50f},
    {.Group = "Torso", .DisplayName = "Hip width",       .Value = &Jgc::LHumanoidShape::HipWidth,       .Min = 0.85f, .Max = 1.30f},
    {.Group = "Torso", .DisplayName = "Shoulder width",  .Value = &Jgc::LHumanoidShape::ShoulderWidth,  .Min = 0.75f, .Max = 1.35f},
    {.Group = "Arms", .DisplayName = "Upper arm length", .Value = &Jgc::LHumanoidShape::UpperArmLength, .Min = 0.75f, .Max = 1.30f},
    {.Group = "Arms", .DisplayName = "Lower arm length", .Value = &Jgc::LHumanoidShape::LowerArmLength, .Min = 0.75f, .Max = 1.30f},
    {.Group = "Arms", .DisplayName = "Hand size",        .Value = &Jgc::LHumanoidShape::HandSize,       .Min = 0.70f, .Max = 1.40f},
    {.Group = "Legs", .DisplayName = "Upper leg length", .Value = &Jgc::LHumanoidShape::UpperLegLength, .Min = 0.80f, .Max = 1.25f},
    {.Group = "Legs", .DisplayName = "Lower leg length", .Value = &Jgc::LHumanoidShape::LowerLegLength, .Min = 0.80f, .Max = 1.25f},
    {.Group = "Legs", .DisplayName = "Foot size",        .Value = &Jgc::LHumanoidShape::FootSize,       .Min = 0.75f, .Max = 1.35f},
};

//# The Rigify names of the deform bones that the shape changes.
constexpr LStringView HeadJoint{ "DEF-spine.006" };
constexpr LStringView FootJoints[]{ "DEF-foot.L", "DEF-foot.R" };

//# The first skeletal mesh in the hierarchy below #Comp. Depth-first.
NODISCARD Jafg::ASkeletalMeshComponent* FindBody(Jafg::ASceneComponent& Comp) noexcept
{
    if (auto* Body{Comp.As<Jafg::ASkeletalMeshComponent>()})
    {
        return Body;
    }
    for (auto const& Child: Comp.GetChildren())
    {
        if (auto* Body{::FindBody(*Child)})
        {
            return Body;
        }
    }
    return nullptr;
}

} /* ~Namespace <Anonymous> */

std::span<Jgc::LHumanoidShapeParameter const> Jgc::GetHumanoidShapeParameters() noexcept
{
    return ::Parameters;
}

void Jgc::AJgcHumanoidShapeComponent::OnAttach(Jafg::AActor& InOwner)
{
    Super::OnAttach(InOwner);
    if (this->Shape != LHumanoidShape{})
    {
        this->Apply();
    }
}

void Jgc::AJgcHumanoidShapeComponent::SetShape(LHumanoidShape const& NewShape)
{
    LHumanoidShape Clamped{NewShape};
    for (LHumanoidShapeParameter const& Parameter: ::Parameters)
    {
        Clamped.*Parameter.Value = maths::clamp(Clamped.*Parameter.Value, Parameter.Min, Parameter.Max);
    }
    if (Clamped == this->Shape)
    {
        return;
    }

    this->Shape = Clamped;
    this->Apply();
}

Jafg::ASkeletalMeshComponent* Jgc::AJgcHumanoidShapeComponent::GetBody() const noexcept
{
    if (!this->IsOwningActorValid() || !this->GetOwningActor().HasRootComponent())
    {
        return nullptr;
    }
    return ::FindBody(const_cast<Jafg::ASceneComponent&>(this->GetOwningActor().GetRootComponent()));
}

void Jgc::AJgcHumanoidShapeComponent::Apply()
{
    Jafg::ASkeletalMeshComponent* const Body{this->GetBody()};
    Jafg::LSkeleton const* const Skeleton{Body ? Body->GetSkeleton() : nullptr};
    if (!Skeleton)
    {
        LOG_VERBOSE(LogJgc, "[{}]: There is no body to shape.", this->GetNameAsString())
        return;
    }

    LHumanoidShape const& S{this->Shape};
    TArray<Jafg::LJointProportion> Proportions(Skeleton->GetJointCount());
    auto Find{[&](LStringView Name) -> std::optional<u32> { return Skeleton->FindJoint(Name); }};
    auto Diagonal{[](LVec3F const& Factors) { return LMat3F{Factors.x, 0.0f, 0.0f, 0.0f, Factors.y, 0.0f, 0.0f, 0.0f, Factors.z}; }};

    //#
    //# Makes bones longer or shorter along themselves (Y). Rigify splits some bones into segments (e.g., for
    //# twisting); all of them are stretched. The joints after them move along (see #Jafg::LJointProportion::Mesh).
    //#
    auto Stretch{[&](std::initializer_list<LString> Segments, f32 Factor)
    {
        if (Factor == 1.0f)
        {
            return;
        }
        for (LString const& Segment: Segments)
        {
            if (std::optional<u32> const Joint{Find(Segment)})
            {
                Proportions[*Joint].Mesh = Diagonal(LVec3F{1.0f, Factor, 1.0f}) * Proportions[*Joint].Mesh;
            }
        }
    }};
    //#
    //# Scales the vertices of joints along the axes of the body: X wide, Y high, Z deep. Whatever the axes of the
    //# joints are (e.g., the pelvis is turned). Around the origin of each joint, which moves with its parent.
    //#
    auto ScaleAlongBody{[&](std::initializer_list<LString> Joints, LVec3F const& Factors)
    {
        /* Turning into the axes of the body and back is not exact. The authored body stays exactly as it is. */
        if (Factors == LVec3F{1.0f})
        {
            return;
        }
        for (LString const& Name: Joints)
        {
            if (std::optional<u32> const Joint{Find(Name)})
            {
                /* The axes of the joint in the bind pose, in the space of the body. */
                LMat3F const Bind{glm::inverse(Skeleton->Joints[*Joint].InverseBind)};
                LMat3F const Axes{maths::normalize(Bind[0]), maths::normalize(Bind[1]), maths::normalize(Bind[2])};
                Proportions[*Joint].Mesh = glm::transpose(Axes) * Diagonal(Factors) * Axes * Proportions[*Joint].Mesh;
            }
        }
    }};
    //# Scales a joint and everything below it uniformly.
    auto Scale{[&](LStringView Name, f32 Factor)
    {
        if (std::optional<u32> const Joint{Find(Name)}; Joint && Factor != 1.0f)
        {
            Proportions[*Joint].Scale *= Factor;
        }
    }};

    for (LStringView const Side: {LStringView{".L"}, LStringView{".R"}})
    {
        auto Name{[Side](LStringView Bone, LStringView Segment = {}) { return algo::sprintf("DEF-{}{}{}", Bone, Side, Segment); }};

        Stretch({Name("shoulder")}, S.ShoulderWidth);
        Stretch({Name("upper_arm"), Name("upper_arm", ".001")}, S.UpperArmLength);
        Stretch({Name("forearm"), Name("forearm", ".001")}, S.LowerArmLength);
        Stretch({Name("thigh"), Name("thigh", ".001")}, S.UpperLegLength);
        Stretch({Name("shin"), Name("shin", ".001")}, S.LowerLegLength);
        /* Uniform, so the fingers and toes below them scale along without being skewed. */
        Scale(Name("hand"), S.HandSize);
        Scale(Name("foot"), S.FootSize);
    }

    /*
     * The torso. The front of the chest and the sides of the hips are moved by the breast and pelvis bones, not by
     * the spine, so they are scaled with it.
     */
    ScaleAlongBody({"DEF-spine", "DEF-spine.001", "DEF-spine.002", "DEF-spine.003", "DEF-breast.L", "DEF-breast.R", "DEF-pelvis.L", "DEF-pelvis.R"}
        , LVec3F{1.0f, S.TorsoLength, 1.0f});
    /* With the upper chest (the collar bones), so the arms move outward with it. */
    ScaleAlongBody({"DEF-spine.002", "DEF-spine.003", "DEF-breast.L", "DEF-breast.R", "DEF-shoulder.L", "DEF-shoulder.R"}
        , LVec3F{S.ChestWidth, 1.0f, S.ChestDepth});
    ScaleAlongBody({"DEF-spine.001"}, LVec3F{S.WaistWidth, 1.0f, S.WaistDepth});
    /* The legs hang below the lowest spine bone. They move outward with it. */
    ScaleAlongBody({"DEF-spine", "DEF-pelvis.L", "DEF-pelvis.R"}, LVec3F{S.HipWidth, 1.0f, 1.0f});

    Stretch({"DEF-spine.004", "DEF-spine.005"}, S.NeckLength);
    /* The size carries over to joints of the face. */
    Scale(HeadJoint, S.HeadSize);
    ScaleAlongBody({LString{HeadJoint}}, LVec3F{S.HeadWidth, S.HeadHeight, S.HeadDepth});

    /* Where the feet are in the rest pose, before and after. The mesh has its origin on the ground. */
    TArray<Jafg::LJointTransform> Rest;
    Skeleton->GetRestPose(Rest);
    TArray<Jafg::LJointTransform> Shaped{Rest};
    Skeleton->ApplyProportions(Shaped, Proportions);
    TArray<LMat4F> RestModel(Rest.size());
    TArray<LMat4F> ShapedModel(Rest.size());
    Skeleton->LocalToModel(Rest, RestModel);
    Skeleton->LocalToModel(Shaped, ShapedModel);

    /* The ground below each ankle moves with its foot (e.g., down for longer legs). The lowest one stands on the ground. */
    std::optional<f32> Lowest;
    for (LStringView const Foot: ::FootJoints)
    {
        if (std::optional<u32> const Joint{Find(Foot)})
        {
            LVec3F const Ankle{RestModel[*Joint][3]};
            f32 const Y{(ShapedModel[*Joint] * glm::inverse(RestModel[*Joint]) * LVec4F{Ankle.x, 0.0f, Ankle.z, 1.0f}).y};
            Lowest = Lowest ? maths::min(*Lowest, Y) : Y;
        }
    }
    f32 const Lift{Lowest ? -*Lowest : 0.0f};

    Body->SetProportions(std::move(Proportions));

    /* Only the mesh moves, not the character. Up in the space of the body is the up of the root. */
    if (Lift != this->AppliedLift)
    {
        Body->SetLocalTranslationByTeleport(Body->GetLocalTranslation()
            + maths::up_vector<LWorldVec3> * static_cast<LWorldReal>(Lift - this->AppliedLift));
        this->AppliedLift = Lift;
    }

    LOG_VERBOSE(LogJgc, "[{}]: Shaped [{}]. The body is lifted by [{:.3f}] m."
        , this->GetNameAsString(), this->GetOwningActor().GetNameAsString(), Lift)
}
