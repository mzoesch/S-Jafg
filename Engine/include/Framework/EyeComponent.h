// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/SceneComponent.h"
#include "EyeComponent.generated.h"

namespace Jafg
{

class APersonaController;

//# The state of the spring of an eye. @see #StepEyeSpring
struct LEyeSpringState final
{
    //# Where the eye is relative to its target. In world space.
    LVec3F Offset{ 0.0f };
    //# The velocity of the eye in world space.
    LVec3F Velocity{ 0.0f };
};

struct LEyeSpringSettings final
{
    //# Natural frequency in Hz.
    f32 Frequency{ 1.0f };
    f32 DampingRatio{ 1.0f };
    //# Half axes of the ellipsoid around the target that the eye stays in: lateral, vertical and longitudinal.
    LVec3F MaxOffset{ 1.0f };
};

//#
//# Advance a spring that pulls an eye to a target that moves with #TargetVelocity. The damping is relative to the
//# target: At a constant velocity the eye settles at the target, a constant acceleration a pushes it a / (2 pi f)^2
//# against the acceleration. It never leaves the ellipsoid of #LEyeSpringSettings::MaxOffset around the target (in
//# the space of #TargetRotation).
//#
//# The offset is kept relative to the target. Moves of the target that its velocity does not explain (e.g., a
//# gimbal that is turned or a teleport) are followed rigidly.
//#
ENGINE_API void StepEyeSpring(
      LEyeSpringState& State
    , LEyeSpringSettings const& Settings
    , LWorldQuat const& TargetRotation
    , LVec3F const& TargetVelocity
    , f32 Dt
    ) noexcept;

//#
//# Advance the lag of a rotation behind a target that turns with #AngularVelocity (world space, radians per
//# second). The presentation stays where it was in the world while the target turns, and catches up with #Speed
//# (1/s). A constant turn rate w makes it lag about w / #Speed behind. Never more than #MaxAngle (radians).
//# @param Lag The rotation from the target to the presentation, in world space: Presentation = Lag * Target.
//#
ENGINE_API void StepEyeRotationLag(LWorldQuat& Lag, LVec3F const& AngularVelocity, f32 Speed, f32 MaxAngle, f32 Dt) noexcept;

//#
//# A point of view in the world, e.g., the head of a character or a camera behind a ship. It looks along its
//# forward vector. A controller sees through the eye of the avatar that it possesses (see #AAvatarComponent::GetEye
//# and #APersonaController::GetView).
//#
//# Eyes are ordinary scene components: Attach them wherever the view should be. A third person camera, for example,
//# is an eye below the eye of the head, moved backwards.
//#
//# There are two transforms:
//#   - The target (#GetTargetEye): Where the eye is supposed to be. The transform of this component.
//#   - The presentation (#GetEye): What viewers see. Equal to the target, unless the eye is sprung (see #SpringFrequency)
//#     or its rotation lags (see #RotationLagSpeed). The presentation is advanced by the controllers that view the
//#     eye, once per frame after the world moved (see #APersonaController::GetView).
//#
//# Spring and lag react to the motion of the body that carries the eye: The spring is damped relative to the target.
//# At a constant velocity the eye settles at the target. Accelerations push it away from the target, at most by the
//# #SpringMaxLateral, #SpringMaxVertical and #SpringMaxLongitudinal distances (an ellipsoid around the target, in the
//# space of the target). Everything else (e.g., turning an #AGimbalComponent above the eye) moves it rigidly. The
//# motion is exact if a simulated rigid body carries the eye, and estimated from the target otherwise.
//#
DECLARE_JAFG_CLASS()
class ENGINE_API AEyeComponent : public ASceneComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(AEyeComponent)

public:

    //# The perspective as presented to viewers (with spring and lag). In world space.
    NODISCARD LWorldEye GetEye() const noexcept;
    //# The perspective where this eye is supposed to be (the transform of this component). In world space.
    NODISCARD LWorldEye GetTargetEye() const noexcept;

    NODISCARD FORCEINLINE constexpr bool IsSprung() const noexcept { return this->SpringFrequency > 0.0f; }
    NODISCARD FORCEINLINE constexpr bool IsRotationLagging() const noexcept { return this->RotationLagSpeed > 0.0f; }

    //#
    //# Advance the presentation by #Dt. Viewers call this once per frame. Further calls in the same frame are ignored,
    //# so that multiple viewers do not speed it up.
    //#
    void UpdatePresentation(f32 Dt);
    //# Put the presentation onto the target and stop all motion of the spring (e.g., after a teleport).
    void SnapPresentation() noexcept;

    //# ---------- Viewers ----------

    //# Whether at least one controller sees through this eye.
    NODISCARD FORCEINLINE constexpr bool IsViewed() const noexcept { return this->ViewerCount > 0; }
    NODISCARD FORCEINLINE constexpr u32 GetViewerCount() const noexcept { return this->ViewerCount; }
    //# Jafg internal methods. Do not use. Called by #APersonaController when it starts or stops seeing through this eye.
    void _AddViewer(APersonaController& Viewer);
    void _RemoveViewer(APersonaController& Viewer);

    //# ---------- Blending ----------

    //#
    //# Blend the view of two eyes. Translation, orientation and all frustum parameters are interpolated.
    //# @param Alpha 0 is #From, 1 is #To.
    //#
    NODISCARD static LWorldEye BlendEyes(LWorldEye const& From, LWorldEye const& To, f32 Alpha) noexcept;
    //# The rotation that looks along the front of #Eye with its up.
    NODISCARD static LWorldQuat RotationOf(LWorldEye const& Eye) noexcept;

    //# ---------- Settings ----------

    //# Vertical field of view in radians.
    CLASS_FIELD(EditorEditable)
    f32 VertFov{ maths::pi_f / 3.0f };
    CLASS_FIELD(EditorEditable)
    f32 NearFrustum{ 0.1f };
    CLASS_FIELD(EditorEditable)
    f32 FarFrustum{ 1.0f };

    //# How long a controller blends to this eye when it starts to see through it. In seconds. Zero cuts.
    CLASS_FIELD(EditorEditable)
    f32 BlendInTime{ 0.0f };

    //#
    //# Whether this eye sits inside the body of its actor (e.g., the head of a character). Bodies of the same actor
    //# only draw what is meant to be seen from inside while a controller sees through this eye (@see
    //# #ASkeletalMeshComponent::FirstPersonBody). Shadows are drawn as usual.
    //#
    CLASS_FIELD(EditorEditable)
    bool bFirstPerson{};

    //# Natural frequency of the spring that pulls the eye to the target. In Hz. Zero disables the spring.
    CLASS_FIELD(EditorEditable)
    f32 SpringFrequency{ 0.0f };
    //# One stops without overshooting. Below one the eye swings past the target before it settles.
    CLASS_FIELD(EditorEditable)
    f32 SpringDampingRatio{ 1.0f };
    //# How far the eye may be away from the target sideways. In meters.
    CLASS_FIELD(EditorEditable)
    f32 SpringMaxLateral{ 1.0f };
    //# How far the eye may be away from the target along up. In meters.
    CLASS_FIELD(EditorEditable)
    f32 SpringMaxVertical{ 1.0f };
    //# How far the eye may be away from the target along forward. In meters.
    CLASS_FIELD(EditorEditable)
    f32 SpringMaxLongitudinal{ 1.0f };

    //# How fast the rotation follows the target. In 1/s. Zero disables the lag.
    CLASS_FIELD(EditorEditable)
    f32 RotationLagSpeed{ 0.0f };
    //# How far the rotation may be behind the target. In degrees.
    CLASS_FIELD(EditorEditable)
    f32 MaxRotationLag{ 15.0f };

    //# ---------- Utilities ----------

    //# The first eye in the hierarchy below and including #Root. Depth first.
    NODISCARD static AEyeComponent* FindFirstEyeIn(ASceneComponent const& Root) noexcept;
    //# Calls #Fn for every eye in the hierarchy below and including #Root. Depth first.
    template<typename TFn> requires std::is_invocable_v<TFn, AEyeComponent&>
    static void ForEachEyeIn(ASceneComponent const& Root, TFn&& Fn)
    {
        if (auto* Eye{const_cast<ASceneComponent&>(Root).As<AEyeComponent>()})
        {
            Fn(*Eye);
        }
        for (auto const& Child: Root.GetChildren())
        {
            ForEachEyeIn(*Child, Fn);
        }
    }

protected:

    //# Called when the first controller starts to see through this eye.
    virtual void OnViewStarted(APersonaController& Viewer) {}
    //# Called when the last controller stops to see through this eye.
    virtual void OnViewEnded(APersonaController& Viewer) {}

private:

    //#
    //# The motion of the target in world space. Exact if the hierarchy is moved by a simulated rigid body,
    //# otherwise estimated from the motion of the target.
    //#
    struct LTargetMotion final
    {
        //# Of the target point.
        LVec3F Linear;
        //# Radians per second around the axis.
        LVec3F Angular;
    };
    NODISCARD LTargetMotion ComputeTargetMotion(LWorldTrans const& Target, f32 Dt) noexcept;
    NODISCARD LWorldEye MakeEye(LWorldVec3 const& Translation, LWorldQuat const& Rotation) const noexcept;

    u32 ViewerCount{};

    bool bHasPresentation{};
    u64 LastPresentedFrame{ std::numeric_limits<u64>::max() };
    LEyeSpringState Spring;
    //# Presentation = RotationLag * Target. @see #StepEyeRotationLag
    LWorldQuat RotationLag{ maths::identity<LWorldQuat> };

    //# For the estimation of the motion of the target.
    LWorldVec3 LastTargetTranslation{ maths::zero_vector<LWorldVec3> };
    LWorldQuat LastTargetRotation{ maths::identity<LWorldQuat> };
    LVec3F EstimatedTargetVelocity{ 0.0f };
};

} /* ~Namespace Jafg */
