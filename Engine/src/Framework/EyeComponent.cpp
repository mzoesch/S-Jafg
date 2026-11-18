// Copyright mzoesch. All rights reserved.

#include "Framework/EyeComponent.h"
#include "Engine/Engine.h"
#include "Framework/PhysicsSystem.h"
#include "Framework/RigidComponent.h"

namespace
{

//# The spring is integrated in steps of at most this length. Keeps stiff springs stable at low frame rates.
inline constexpr f32 MaxSpringStep{ 1.0f / 240.0f };
//# How fast the estimated target velocity follows the measured one. In seconds.
inline constexpr f32 TargetVelocitySmoothing{ 0.08f };

NODISCARD LWorldQuat RotationFromBasis(LVec3F const& Front, LVec3F const& Up) noexcept
{
    LVec3F const Z{maths::normalize(Front)};
    LVec3F Y{Up - Z * maths::dot(Up, Z)};
    f32 const Length{maths::magnitude(Y)};
    Y = Length > 1.0e-6f ? Y / Length : (std::abs(Z.y) < 0.99f ? maths::up_vector<LVec3F> : maths::forward_vector<LVec3F>);
    Y = maths::normalize(Y - Z * maths::dot(Y, Z));
    return maths::normalize(glm::quat_cast(LMat3F{maths::cross(Y, Z), Y, Z}));
}

} /* ~Namespace <Anonymous> */

void Jafg::StepEyeSpring(
      LEyeSpringState& State
    , LEyeSpringSettings const& Settings
    , LWorldVec3 const& Target
    , LWorldQuat const& TargetRotation
    , LVec3F const& TargetVelocity
    , f32 Dt
    ) noexcept
{
    if (Dt <= 0.0f)
    {
        return;
    }

    f32 const Omega{2.0f * maths::pi_f * maths::max(Settings.Frequency, 1.0e-3f)};
    f32 const Zeta{maths::max(Settings.DampingRatio, 0.0f)};
    auto const Steps{static_cast<i32>(std::ceil(Dt / ::MaxSpringStep))};
    f32 const Step{Dt / static_cast<f32>(Steps)};

    /*
     * Integrated relative to the target, which moved along its velocity during this frame. The numbers stay small,
     * so that the spring is as precise far away from the origin as close to it.
     * A change of the target velocity since the last frame (an acceleration) shows up as relative velocity.
     */
    LVec3F Offset{LVec3F{State.Translation - (Target - LWorldVec3{TargetVelocity * Dt})}};
    LVec3F Relative{State.Velocity - TargetVelocity};
    for (i32 Idx{0}; Idx < Steps; ++Idx)
    {
        Relative += (-Omega * Omega * Offset - 2.0f * Zeta * Omega * Relative) * Step;
        Offset += Relative * Step;
    }

    /* Keep the eye inside the ellipsoid around the target. In the space of the target. */
    LVec3F const Max{maths::max(Settings.MaxOffset, LVec3F{1.0e-3f})};
    LVec3F const Local{maths::inverse(TargetRotation) * Offset};
    LVec3F const Scaled{Local / Max};
    if (f32 const Extent{maths::magnitude(Scaled)}; Extent > 1.0f)
    {
        Offset = TargetRotation * (Local / Extent);

        /* Do not keep pushing outwards. Otherwise, the eye sticks to the border after the acceleration ended. */
        LVec3F const Normal{maths::normalize(TargetRotation * (Scaled / Max))};
        if (f32 const Outwards{maths::dot(Relative, Normal)}; Outwards > 0.0f)
        {
            Relative -= Normal * Outwards;
        }
    }

    State.Translation = Target + LWorldVec3{Offset};
    State.Velocity = TargetVelocity + Relative;
}

LWorldEye Jafg::AEyeComponent::GetEye() const noexcept
{
    if (!this->bHasPresentation)
    {
        return this->GetTargetEye();
    }
    return this->MakeEye(this->Spring.Translation, this->PresentedRotation);
}

LWorldEye Jafg::AEyeComponent::GetTargetEye() const noexcept
{
    LWorldTrans const World{this->GetWorldTransformSlow()};
    return this->MakeEye(World.t, World.r);
}

LWorldEye Jafg::AEyeComponent::MakeEye(LWorldVec3 const& Translation, LWorldQuat const& Rotation) const noexcept
{
    LWorldQuat const Normalized{maths::normalize(Rotation)};
    return {
        .vert_fov = this->VertFov,
        .near_frustum = this->NearFrustum,
        .far_frustum = this->FarFrustum,
        .translation = Translation,
        .front = Normalized * maths::forward_vector<LWorldVec3>,
        .up = Normalized * maths::up_vector<LWorldVec3>,
        };
}

void Jafg::AEyeComponent::UpdatePresentation(f32 Dt)
{
    u64 const Frame{this->GetEngine().FrameCount};
    if (this->LastPresentedFrame == Frame)
    {
        return;
    }
    this->LastPresentedFrame = Frame;

    if (!this->IsSprung() && !this->IsRotationLagging())
    {
        this->bHasPresentation = false;
        return;
    }

    LWorldTrans const Target{this->GetWorldTransformSlow()};
    LWorldQuat const TargetRotation{maths::normalize(Target.r)};
    if (!this->bHasPresentation)
    {
        this->SnapPresentation();
        return;
    }
    if (Dt <= 0.0f)
    {
        return;
    }

    LVec3F const TargetVelocity{this->ComputeTargetVelocity(Target.t, Dt)};

    //# ---------- Translation ----------

    if (this->IsSprung())
    {
        Jafg::StepEyeSpring(this->Spring, {
            .Frequency = this->SpringFrequency,
            .DampingRatio = this->SpringDampingRatio,
            .MaxOffset = {this->SpringMaxLateral, this->SpringMaxVertical, this->SpringMaxLongitudinal},
            }, Target.t, TargetRotation, TargetVelocity, Dt);
    }
    else
    {
        this->Spring = {.Translation = Target.t, .Velocity = TargetVelocity};
    }

    //# ---------- Rotation ----------

    if (this->IsRotationLagging())
    {
        f32 const Alpha{1.0f - std::exp(-this->RotationLagSpeed * Dt)};
        LWorldQuat Rotation{maths::normalize(glm::slerp(this->PresentedRotation, TargetRotation, Alpha))};

        f32 const MaxAngle{maths::radians(maths::clamp(this->MaxRotationLag, 0.0f, 180.0f))};
        LWorldQuat const Difference{maths::normalize(Rotation * maths::inverse(TargetRotation))};
        f32 const Angle{2.0f * std::acos(maths::clamp(std::abs(Difference.w), 0.0f, 1.0f))};
        if (Angle > MaxAngle && Angle > 0.0f)
        {
            /* Rotate back from the target by exactly the maximum. */
            Rotation = maths::normalize(glm::slerp(TargetRotation, Rotation, MaxAngle / Angle));
        }
        this->PresentedRotation = Rotation;
    }
    else
    {
        this->PresentedRotation = TargetRotation;
    }
}

void Jafg::AEyeComponent::SnapPresentation() noexcept
{
    if (!this->IsSprung() && !this->IsRotationLagging())
    {
        this->bHasPresentation = false;
        return;
    }

    LWorldTrans const Target{this->GetWorldTransformSlow()};
    this->Spring.Translation = Target.t;
    this->PresentedRotation = maths::normalize(Target.r);
    this->LastTargetTranslation = Target.t;
    this->bHasPresentation = true;

    /* Moves with the target from the start. Otherwise, it would fall behind a moving target right away. */
    this->EstimatedTargetVelocity = LVec3F{0.0f};
    this->Spring.Velocity = this->ComputeTargetVelocity(Target.t, 0.0f);
}

LVec3F Jafg::AEyeComponent::ComputeTargetVelocity(LWorldVec3 const& Target, f32 Dt) noexcept
{
    /*
     * Simulated bodies only move in physics steps, which are usually not in sync with the frames. Their velocity is
     * exact, while differences of their translation between frames jump.
     */
    ASceneComponent* Top{this};
    while (ASceneComponent* Parent{Top->GetTransformParent()})
    {
        Top = Parent;
    }
    if (auto* Rigid{Top->As<ARigidComponent>()}; Rigid && Rigid->IsAddedToSimulation())
    {
        this->LastTargetTranslation = Target;
        return Rigid->GetParentPhysicsSystem().GetBodyInterfaceNoLock().GetPointVelocity(Rigid->GetRigidObject().GetID(), Target);
    }

    if (Dt > 0.0f)
    {
        LVec3F const Measured{LVec3F{Target - this->LastTargetTranslation} / Dt};
        this->EstimatedTargetVelocity += (Measured - this->EstimatedTargetVelocity) * (1.0f - std::exp(-Dt / ::TargetVelocitySmoothing));
    }
    this->LastTargetTranslation = Target;
    return this->EstimatedTargetVelocity;
}

void Jafg::AEyeComponent::_AddViewer(APersonaController& Viewer)
{
    if (this->ViewerCount++ == 0)
    {
        /* Nobody advanced the presentation while nobody looked. */
        this->SnapPresentation();
        this->OnViewStarted(Viewer);
    }
}

void Jafg::AEyeComponent::_RemoveViewer(APersonaController& Viewer)
{
    check(this->ViewerCount > 0)
    if (--this->ViewerCount == 0)
    {
        this->OnViewEnded(Viewer);
    }
}

LWorldEye Jafg::AEyeComponent::BlendEyes(LWorldEye const& From, LWorldEye const& To, f32 Alpha) noexcept
{
    f32 const T{maths::clamp(Alpha, 0.0f, 1.0f)};
    LWorldQuat const Rotation{glm::slerp(
          ::RotationFromBasis(LVec3F{From.front}, LVec3F{From.up})
        , ::RotationFromBasis(LVec3F{To.front}, LVec3F{To.up})
        , T
        )};
    return {
        .vert_fov = From.vert_fov + (To.vert_fov - From.vert_fov) * T,
        .near_frustum = From.near_frustum + (To.near_frustum - From.near_frustum) * T,
        .far_frustum = From.far_frustum + (To.far_frustum - From.far_frustum) * T,
        .translation = From.translation + (To.translation - From.translation) * static_cast<LWorldReal>(T),
        .front = Rotation * maths::forward_vector<LWorldVec3>,
        .up = Rotation * maths::up_vector<LWorldVec3>,
        };
}

LWorldQuat Jafg::AEyeComponent::RotationOf(LWorldEye const& Eye) noexcept
{
    return ::RotationFromBasis(LVec3F{Eye.front}, LVec3F{Eye.up});
}

Jafg::AEyeComponent* Jafg::AEyeComponent::FindFirstEyeIn(ASceneComponent const& Root) noexcept
{
    if (auto* Eye{const_cast<ASceneComponent&>(Root).As<AEyeComponent>()})
    {
        return Eye;
    }
    for (auto const& Child: Root.GetChildren())
    {
        if (auto* Eye{FindFirstEyeIn(*Child)})
        {
            return Eye;
        }
    }
    return nullptr;
}
