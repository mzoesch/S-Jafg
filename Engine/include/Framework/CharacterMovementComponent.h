// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/MovementComponent.h"
#include "CharacterMovementComponent.generated.h"

namespace Jafg
{

namespace Detail
{
struct LCharacterMovementProxy;
} /* ~Namespace Detail */

//#
//# Moves a pawn as a character. Backed by a Jolt virtual character: a kinematic capsule that collides and slides
//# through the world, but is not simulated by the physics system itself.
//#
//# Supports:
//#   - Walking on slopes and stairs, sticking to the floor and standing on moving bodies (e.g., ships). The pawn
//#     inherits the velocity of the ground and turns with it.
//#   - Falling with air control, coyote time and jump buffering.
//#   - Crouching. Standing up again is blocked while there is no room for it.
//#   - Flying with a jetpack in gravity. With engaged dampeners the jetpack hovers and brakes on its own.
//#   - 6DOF zero-g movement (EVA) with thrusters and optional inertial dampeners.
//#   - Magnetic boots to walk on any surface in (near) zero gravity. Jumping pushes off.
//#   - Arbitrary gravity (#ASupremePolicies::GetGravityAt). The body aligns to the gravity smoothly.
//#   - Collisions with other characters and pushing of dynamic bodies. Other bodies can collide with and push the
//#     character through an inner kinematic body.
//#
//# The root component of the owning pawn is the location of the feet. Its rotation is the body rotation, which stays
//# upright in gravity. The view (#APawn::GetViewLocalRotation) additionally pitches relative to the body.
//#
DECLARE_JAFG_CLASS()
class ENGINE_API ACharacterMovementComponent : public AMovementComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(ACharacterMovementComponent)

public:

    virtual void OnAttach(AActor& InOwner) override;
    virtual void OnGarbage(EJxxRecordTearDownReason Reason) override;

    virtual LVec3F GetVelocity() const noexcept override;
    virtual LVec3F GetUp() const noexcept override;
    virtual void Teleport(LWorldVec3 const& Location, std::optional<LWorldQuat> Rotation = {}) override;
    virtual LMovementSnapshot CaptureSnapshot() const override;

    //# ---------- Queries ----------

    NODISCARD FORCEINLINE constexpr bool IsCrouched() const noexcept { return !!(this->StateFlags & EMovementStateBits::Crouched); }
    //# Whether the character stands on walkable ground.
    NODISCARD bool IsOnGround() const noexcept;
    //# Normal of the ground that is touched. Zero if nothing is touched.
    NODISCARD LVec3F GetGroundNormal() const noexcept;
    //# Velocity of the ground that is touched (e.g., a moving ship). Zero if nothing is touched.
    NODISCARD LVec3F GetGroundVelocity() const noexcept;
    NODISCARD LWorldQuat GetBodyRotation() const noexcept;
    NODISCARD FORCEINLINE constexpr f32 GetViewPitch() const noexcept { return this->ViewPitch; }
    //# The gravity that was used in the previous tick.
    NODISCARD FORCEINLINE constexpr LVec3F const& GetGravity() const noexcept { return this->LastGravity; }
    NODISCARD FORCEINLINE constexpr f32 GetStandingHeight() const noexcept { return this->StandingHeight; }
    NODISCARD FORCEINLINE constexpr f32 GetCrouchingHeight() const noexcept { return this->CrouchingHeight; }
    NODISCARD FORCEINLINE constexpr f32 GetCapsuleRadius() const noexcept { return this->CapsuleRadius; }

    //# Change the dimensions of the capsule. The character will stand up if there is room for it.
    void SetCapsuleDimensions(f32 InStandingHeight, f32 InCrouchingHeight, f32 InRadius);

    //# ---------- Tunables ----------
    //# Distances are in meters, speeds in meters per second, accelerations in meters per second squared and angles
    //# in degrees unless stated otherwise.

    CLASS_FIELD(EditorEditable)
    f32 StandingEyeHeight{ 1.62f };
    CLASS_FIELD(EditorEditable)
    f32 CrouchingEyeHeight{ 1.02f };
    //# How fast the eye follows crouching. In 1/s.
    CLASS_FIELD(EditorEditable)
    f32 EyeHeightBlendSpeed{ 12.0f };
    CLASS_FIELD(EditorEditable)
    f32 MaxViewPitch{ 89.0f };

    //# In kg. Used to push down bodies that the character stands on.
    CLASS_FIELD(EditorEditable)
    f32 Mass{ 80.0f };
    //# The maximum force with which the character pushes other bodies. In N.
    CLASS_FIELD(EditorEditable)
    f32 MaxPushForce{ 500.0f };
    CLASS_FIELD(EditorEditable)
    bool bCanPushBodies{ true };
    CLASS_FIELD(EditorEditable)
    bool bCanBePushedByBodies{ true };

    CLASS_FIELD(EditorEditable)
    f32 MaxSlopeAngle{ 50.0f };
    CLASS_FIELD(EditorEditable)
    f32 MaxStepHeight{ 0.4f };
    //# How far the character snaps down to stay on the floor when walking down slopes or stairs.
    CLASS_FIELD(EditorEditable)
    f32 StickToFloorDistance{ 0.5f };
    //# Whether the character turns with the body it stands on.
    CLASS_FIELD(EditorEditable)
    bool bFollowBaseRotation{ true };

    CLASS_FIELD(EditorEditable)
    f32 WalkSpeed{ 4.0f };
    CLASS_FIELD(EditorEditable)
    f32 SprintSpeed{ 7.0f };
    CLASS_FIELD(EditorEditable)
    f32 CrouchSpeed{ 2.0f };
    CLASS_FIELD(EditorEditable)
    f32 GroundAcceleration{ 50.0f };
    //# Deceleration on the ground without input.
    CLASS_FIELD(EditorEditable)
    f32 GroundBraking{ 40.0f };
    CLASS_FIELD(EditorEditable)
    f32 JumpSpeed{ 5.0f };
    //# How long a jump is still possible after walking off a ledge. In seconds.
    CLASS_FIELD(EditorEditable)
    f32 CoyoteTime{ 0.12f };
    //# How long a jump press is remembered before landing. In seconds.
    CLASS_FIELD(EditorEditable)
    f32 JumpBufferTime{ 0.15f };

    //# The maximum horizontal speed that air control accelerates to.
    CLASS_FIELD(EditorEditable)
    f32 AirSpeed{ 4.0f };
    CLASS_FIELD(EditorEditable)
    f32 AirAcceleration{ 10.0f };
    //# Linear drag while falling (e.g., atmosphere). In 1/s. Zero disables drag.
    CLASS_FIELD(EditorEditable)
    f32 FallingLinearDrag{ 0.0f };

    CLASS_FIELD(EditorEditable)
    f32 JetpackAcceleration{ 10.0f };
    CLASS_FIELD(EditorEditable)
    f32 JetpackMaxSpeed{ 12.0f };
    //# Deceleration of engaged dampeners while flying. Must exceed the gravity to hover.
    CLASS_FIELD(EditorEditable)
    f32 JetpackDampenerDeceleration{ 15.0f };

    //# Below this gravity, the character is weightless.
    CLASS_FIELD(EditorEditable)
    f32 ZeroGravityThreshold{ 0.05f };
    CLASS_FIELD(EditorEditable)
    f32 EvaAcceleration{ 5.0f };
    CLASS_FIELD(EditorEditable)
    f32 EvaMaxSpeed{ 15.0f };
    CLASS_FIELD(EditorEditable)
    f32 EvaDampenerDeceleration{ 5.0f };

    CLASS_FIELD(EditorEditable)
    f32 MagBootsWalkSpeed{ 2.5f };
    //# Acceleration towards the surface while the boots are attached or in reach.
    CLASS_FIELD(EditorEditable)
    f32 MagBootsAdhesion{ 9.81f };
    //# How far below the feet a surface may be to be attracted by the boots.
    CLASS_FIELD(EditorEditable)
    f32 MagBootsReach{ 1.0f };
    //# The boots only work if the gravity is at most this strong.
    CLASS_FIELD(EditorEditable)
    f32 MagBootsMaxGravity{ 1.0f };
    CLASS_FIELD(EditorEditable)
    f32 MagBootsPushOffSpeed{ 2.5f };
    //# How long the boots do not attach again after pushing off. In seconds.
    CLASS_FIELD(EditorEditable)
    f32 MagBootsReattachDelay{ 0.5f };
    //# In radians per second.
    CLASS_FIELD(EditorEditable)
    f32 MagBootsAlignRate{ 3.0f };

    //# How fast the body aligns to a changed gravity direction. In radians per second.
    CLASS_FIELD(EditorEditable)
    f32 UpAlignmentRate{ 4.0f };

protected:

    virtual void SimulateTick(LMovementInput const& Input, f32 Dt) override;
    virtual void RestoreSnapshot(LMovementSnapshot const& Snapshot) override;
    virtual void UpdatePresentation(f32 Dt) override;
    virtual LVec3F WorldDirectionToMoveAxes(LVec3F const& Direction) const noexcept override;
    virtual LVec3F ComputeLookDeltaTowards(LVec3F const& Direction, f32 MaxAngle) const noexcept override;

    //# ---------- Hooks ----------
    //# All hooks that affect the simulation must be deterministic for prediction to work.

    //# The gravity at a location. Defaults to #ASupremePolicies::GetGravityAt.
    NODISCARD virtual LVec3F ComputeGravity(LWorldVec3 const& Location) const;
    //# The velocity of the local reference frame that thrusters and dampeners work relative to. Zero by default.
    NODISCARD virtual LVec3F GetReferenceFrameVelocity() const { return LVec3F{0.0f}; }
    //# Scales all thruster accelerations (jetpack, EVA) for this tick in [0, 1]. Use for fuel. One by default.
    NODISCARD virtual f32 GetThrusterEfficiency(LMovementInput const& Input, f32 Dt) const { return 1.0f; }
    //# Called with the velocity change in m/s that thrusters caused in this tick. Use to consume fuel.
    virtual void OnThrustersUsed(f32 DeltaVelocity, f32 Dt) {}
    //# Cosmetic. Not called during replays.
    virtual void OnJumped() {}
    //# Cosmetic. Not called during replays. #ImpactVelocity is relative to the ground.
    virtual void OnLanded(LVec3F const& ImpactVelocity) {}

    //# Additional deterministic state for subclasses. It is part of every snapshot.
    NODISCARD FORCEINLINE constexpr std::array<f32, 4>& GetCustomState() noexcept { return this->CustomState; }
    NODISCARD FORCEINLINE constexpr std::array<f32, 4> const& GetCustomState() const noexcept { return this->CustomState; }

private:

    void CreateCharacter();
    void DestroyCharacter() noexcept;
    void RebuildShapes();
    //# Change between the standing and the crouching capsule. @return Whether the shape is now as requested.
    bool TrySetCrouched(bool bCrouch, bool bForce);
    void ApplyRuntimeSettings() noexcept;
    //# Adopt changes to the root component that were made by someone else (e.g., teleports by game code).
    void SyncExternalChanges() noexcept;
    void FollowBase() noexcept;
    void ApplyLook(LVec3F const& Look, EMovementMode Mode) noexcept;
    //# Rotate the body by the world space #Delta around the eye, so that the view does not move.
    void RotateBodyAroundEye(LWorldQuat const& Delta) noexcept;
    //# When the body becomes free to rotate, the view pitch becomes a part of the body rotation.
    void FoldViewPitchIntoBody() noexcept;
    NODISCARD f32 GetSimulatedEyeHeight() const noexcept;

    struct LSurface final
    {
        LVec3F Normal;
        bool bTouching{};
    };
    NODISCARD std::optional<LSurface> FindMagneticSurface() const;

    //# The flat (relative to up) direction that the move axes point to. The length is at most one.
    NODISCARD LVec3F GetFlatWishDirection(LVec3F const& Axes) const noexcept;

    using LProxyPtr = TUnique<Detail::LCharacterMovementProxy, void(*)(Detail::LCharacterMovementProxy*)>;
    LProxyPtr Proxy{nullptr, nullptr};

    f32 StandingHeight{ 1.8f };
    f32 CrouchingHeight{ 1.2f };
    f32 CapsuleRadius{ 0.3f };

    //# ---------- Simulated state ----------
    //# Everything in here must be part of the snapshots.

    EMovementStateFlags StateFlags{};
    f32 ViewPitch{};
    f32 CoyoteTimeRemaining{};
    f32 JumpBufferRemaining{};
    f32 MagBootsCooldownRemaining{};
    u32 BaseBodyId{ ~0u };
    LWorldQuat BaseRotation{ maths::identity<LWorldQuat> };
    std::array<f32, 4> CustomState{};

    //# ---------- Cached / presentation state ----------

    LVec3F LastGravity{ 0.0f };
    f32 PresentedEyeHeight{ -1.0f };
    bool bHasWrittenRoot{};
    LWorldVec3 LastWrittenTranslation{ 0.0f };
    LWorldQuat LastWrittenRotation{ maths::identity<LWorldQuat> };
};

} /* ~Namespace Jafg */
