// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

//#
//# Shared vocabulary for all movement components.
//#
//# Conventions used by every movement type in here:
//#   - Local movement axes are view-relative: X is right, Y is up and Z is forward.
//#     Note that in body space "right" is #maths::left_vector (-X), as the engine is right-handed with +Z forward
//#     and +Y up. Use #Movement::MoveAxesToBody and #Movement::BodyToMoveAxes to convert.
//#   - Look deltas are in radians: X is yaw (positive turns right), Y is pitch (positive looks up) and Z is roll
//#     (positive rolls right).
//#
#if PREPROCESSOR_EXCLUDE_FF
#endif /* PREPROCESSOR_EXCLUDE_FF */

namespace Jafg
{

//#
//# The rule set that is currently used to integrate the motion of a character.
//#
SERDE_DECLARE_TRIVIAL_ENUM(EMovementMode, u8,
    //# Not simulated. The character is frozen in place.
    None,
    //# Standing on walkable ground while being affected by gravity.
    Walking,
    //# Airborne while being affected by gravity without any propulsion.
    Falling,
    //# Thrust-based flight while being affected by gravity (e.g., a jetpack on a planet).
    Flying,
    //# Weightless 6DOF movement without relevant gravity (e.g., an EVA in space).
    ZeroG,
    //# Standing on an arbitrary surface with magnetic boots in (near) zero gravity. Up is the surface normal.
    MagWalking,
    //# Reserved for subclasses.
    Custom
    )

//#
//# Whether a mode keeps the body upright with respect to an up vector (gravity or a surface).
//# Bodies that are not upright are free to rotate in all three axes.
//#
NODISCARD constexpr bool IsUprightMovementMode(EMovementMode Mode) noexcept
{
    return Mode == EMovementMode::Walking
        || Mode == EMovementMode::Falling
        || Mode == EMovementMode::Flying
        || Mode == EMovementMode::MagWalking;
}

//# Whether a mode stands on a surface.
NODISCARD constexpr bool IsGroundedMovementMode(EMovementMode Mode) noexcept
{
    return Mode == EMovementMode::Walking || Mode == EMovementMode::MagWalking;
}

//#
//# Who owns the simulation of a movement component.
//# Movement is server authoritative. The owning client predicts and reconciles.
//#
enum struct EMovementNetRole: u8
{
    //#
    //# Standalone or server. Simulates authoritatively.
    //# The input either comes from the local intent (local players, bots) or, if the character is remotely controlled, from
    //# input commands that were received from the owning client.
    //#
    Authority,
    //# The owning client. Predicts from the local intent and reconciles with authoritative snapshots.
    AutonomousProxy,
    //# A non-owning client. Does not simulate at all and only follows authoritative snapshots.
    SimulatedProxy,
};
SERDE_ENUM(EMovementNetRole, Authority, AutonomousProxy, SimulatedProxy)

enum struct EMovementButtonBits: u16
{
    Identity     = 0,
    //# Edge. Jump was pressed since the previous command.
    JumpPressed  = 1 << 0,
    //# Level. Jump is held. Used for upwards thrust while flying.
    JumpHeld     = 1 << 1,
    //# Level. Crouch is held. Used for downwards thrust while flying.
    Crouch       = 1 << 2,
    //# Level.
    Sprint       = 1 << 3,
    //# State. The jetpack is engaged. Airborne characters in gravity will then fly instead of fall.
    Jetpack      = 1 << 4,
    //# State. The inertial dampeners are engaged. Thrusters will then cancel any motion that is not requested.
    Dampeners    = 1 << 5,
    //# State. The magnetic boots are engaged. Allows to stand on any surface in (near) zero gravity.
    MagBoots     = 1 << 6,
};
ENUM_STRUCT_FLAGS(EMovementButtonBits, EMovementButtonFlags)

//#
//# A single input command. One command is consumed per physics tick.
//#
//# This is the unit that will be sent from a client to the server. It must therefore only contain what the
//# controlling entity wants, never results of the simulation.
//#
struct LMovementInput final
{
    //# Monotonically increasing per produced command. Zero is never a valid sequence.
    u32 Sequence{};
    //# Local movement axes. Each in [-1, 1]. @see the conventions at the top of this file.
    LVec3F Move{ 0.0f };
    //# Look delta in radians since the previous command. @see the conventions at the top of this file.
    LVec3F Look{ 0.0f };
    EMovementButtonFlags Buttons{};

    NODISCARD FORCEINLINE constexpr bool Has(EMovementButtonBits Bit) const noexcept { return !!(this->Buttons & Bit); }
};

//# Flags that are part of the simulated state.
enum struct EMovementStateBits: u16
{
    Identity   = 0,
    Crouched   = 1 << 0,
    Jetpack    = 1 << 1,
    Dampeners  = 1 << 2,
    MagBoots   = 1 << 3,
    //# Set for the tick in which a jump was executed. Prevents sticking to the floor.
    JustJumped = 1 << 4,
};
ENUM_STRUCT_FLAGS(EMovementStateBits, EMovementStateFlags)

//#
//# The complete simulated state of a movement component at the end of a tick.
//# Restoring a snapshot and re-simulating the same inputs must reproduce the same result. This is the basis for
//# client-side prediction and server reconciliation.
//#
struct LMovementSnapshot final
{
    //# The sequence of the last input command that is included in this state.
    u32 Sequence{};
    EMovementMode Mode{ EMovementMode::Falling };
    EMovementStateFlags Flags{};
    //# Location of the feet.
    LWorldVec3 Position{ 0.0f };
    LWorldQuat Rotation{ maths::identity<LWorldQuat> };
    LVec3F Velocity{ 0.0f };
    LVec3F Up{ maths::up_vector<LVec3F> };
    //# Pitch of the view relative to the body (only used in upright modes). In radians.
    f32 ViewPitch{};
    //# Remaining time in which a jump is still allowed after leaving the ground.
    f32 CoyoteTime{};
    //# Remaining time in which a jump press is remembered (allows pressing jump shortly before landing).
    f32 JumpBuffer{};
    //# Remaining time in which the magnetic boots will not attach to a surface again.
    f32 MagBootsCooldown{};
    //# Physics body that the character stands on. Used to follow the rotation of moving bases (e.g., ships).
    u32 BaseBodyId{ ~0u };
    LWorldQuat BaseRotation{ maths::identity<LWorldQuat> };
    //# Free for subclasses (e.g., jetpack fuel). Must be deterministic for prediction.
    std::array<f32, 4> Custom{};

    NODISCARD FORCEINLINE constexpr bool Has(EMovementStateBits Bit) const noexcept { return !!(this->Flags & Bit); }
};

namespace Movement
{

//# Removes the component of #V that is parallel to the normalized #Normal.
NODISCARD inline LVec3F ProjectOnPlane(LVec3F const& V, LVec3F const& Normal) noexcept
{
    return V - Normal * maths::dot(V, Normal);
}

//# @return #V normalized or #Fallback if #V is (nearly) zero.
NODISCARD inline LVec3F NormalizeOr(LVec3F const& V, LVec3F const& Fallback) noexcept
{
    f32 const Length2{maths::squared_magnitude(V)};
    if (Length2 < maths::squared(maths::not_so_small_number_f))
    {
        return Fallback;
    }
    return V / maths::sqrt(Length2);
}

//# @return #V with its length clamped to at most #MaxLength.
NODISCARD inline LVec3F ClampMagnitude(LVec3F const& V, f32 MaxLength) noexcept
{
    f32 const Length2{maths::squared_magnitude(V)};
    if (Length2 > maths::squared(MaxLength) && Length2 > 0.0f)
    {
        return V * (MaxLength / maths::sqrt(Length2));
    }
    return V;
}

//# @return Some unit vector perpendicular to the normalized #V.
NODISCARD inline LVec3F AnyPerpendicular(LVec3F const& V) noexcept
{
    LVec3F const Other{maths::abs(V.x) < 0.9f ? LVec3F{1.0f, 0.0f, 0.0f} : LVec3F{0.0f, 1.0f, 0.0f}};
    return maths::normalize(maths::cross(V, Other));
}

//# Moves #Current towards #Target by at most #MaxDelta (vector length).
NODISCARD inline LVec3F MoveTowards(LVec3F const& Current, LVec3F const& Target, f32 MaxDelta) noexcept
{
    LVec3F const Delta{Target - Current};
    f32 const Distance2{maths::squared_magnitude(Delta)};
    if (Distance2 <= maths::squared(MaxDelta) || Distance2 == 0.0f)
    {
        return Target;
    }
    return Current + Delta * (MaxDelta / maths::sqrt(Distance2));
}

//#
//# Accelerates #Velocity along the normalized #WishDirection, but never beyond #WishSpeed along that direction.
//# Velocity that is perpendicular to #WishDirection is left untouched, which preserves momentum.
//#
NODISCARD inline LVec3F Accelerate(LVec3F const& Velocity, LVec3F const& WishDirection, f32 WishSpeed, f32 Acceleration, f32 Dt) noexcept
{
    f32 const Current{maths::dot(Velocity, WishDirection)};
    f32 const Missing{WishSpeed - Current};
    if (Missing <= 0.0f)
    {
        return Velocity;
    }
    return Velocity + WishDirection * maths::min(Missing, Acceleration * Dt);
}

//#
//# Decelerates all motion that is not along the normalized (or zero) #ThrustDirection by #Deceleration.
//# Motion along #ThrustDirection is kept, motion against it is cancelled.
//#
NODISCARD inline LVec3F Dampen(LVec3F const& Velocity, LVec3F const& ThrustDirection, f32 Deceleration, f32 Dt) noexcept
{
    LVec3F const Kept{ThrustDirection * maths::max(maths::dot(Velocity, ThrustDirection), 0.0f)};
    return Kept + MoveTowards(Velocity - Kept, LVec3F{0.0f}, Deceleration * Dt);
}

//# Shortest arc rotation that rotates the normalized #From onto the normalized #To.
NODISCARD inline LWorldQuat FromToRotation(LVec3F const& From, LVec3F const& To) noexcept
{
    f32 const Cos{maths::dot(From, To)};
    if (Cos >= 1.0f - maths::not_so_small_number_f)
    {
        return maths::identity<LWorldQuat>;
    }
    if (Cos <= -1.0f + maths::not_so_small_number_f)
    {
        return maths::angle_axis(maths::pi_f, AnyPerpendicular(From));
    }
    LVec3F const Axis{maths::cross(From, To)};
    /* Half way quaternion. Note that quaternions are constructed as (x, y, z, w) in this engine. */
    return maths::normalize(LWorldQuat{Axis.x, Axis.y, Axis.z, 1.0f + Cos});
}

//# Rotates the normalized #From towards the normalized #To by at most #MaxAngle radians.
NODISCARD inline LVec3F RotateTowards(LVec3F const& From, LVec3F const& To, f32 MaxAngle) noexcept
{
    f32 const Angle{maths::acos(maths::clamp(maths::dot(From, To), -1.0f, 1.0f))};
    if (Angle <= MaxAngle || Angle <= maths::not_so_small_number_f)
    {
        return To;
    }
    LVec3F Axis{maths::cross(From, To)};
    Axis = NormalizeOr(Axis, AnyPerpendicular(From));
    return maths::normalize(maths::angle_axis(MaxAngle, Axis) * From);
}

//# The part of #Rotation that rotates around the normalized #Axis (swing-twist decomposition).
NODISCARD inline LWorldQuat ExtractTwist(LWorldQuat const& Rotation, LVec3F const& Axis) noexcept
{
    LVec3F const Projected{Axis * maths::dot(LVec3F{Rotation.x, Rotation.y, Rotation.z}, Axis)};
    LWorldQuat const Twist{Projected.x, Projected.y, Projected.z, Rotation.w};
    f32 const Length2{Twist.x * Twist.x + Twist.y * Twist.y + Twist.z * Twist.z + Twist.w * Twist.w};
    if (Length2 < maths::small_number_f)
    {
        return maths::identity<LWorldQuat>;
    }
    return maths::normalize(Twist);
}

//#
//# Builds a rotation whose forward (+Z) points along #Forward and whose up (+Y) points along #Up.
//# #Forward is orthogonalized against #Up. Both must be normalized; #Forward must not be parallel to #Up.
//#
NODISCARD inline LWorldQuat LookRotation(LVec3F const& Forward, LVec3F const& Up) noexcept
{
    LVec3F const Z{maths::normalize(ProjectOnPlane(Forward, Up))};
    LVec3F const X{maths::cross(Up, Z)};
    return maths::normalize(glm::quat_cast(LMat3F{X, Up, Z}));
}

//# @return The rotation #Rotation turned around #Up such that it is upright relative to #Up and keeps its heading.
NODISCARD inline LWorldQuat MakeUpright(LWorldQuat const& Rotation, LVec3F const& Up) noexcept
{
    LVec3F const Forward{Rotation * maths::forward_vector<LVec3F>};
    LVec3F Heading{ProjectOnPlane(Forward, Up)};
    if (maths::squared_magnitude(Heading) < 1.0e-6f)
    {
        /* Looking straight up or down. Use where the head points to as the heading. */
        Heading = ProjectOnPlane(Rotation * (maths::dot(Forward, Up) > 0.0f ? maths::down_vector<LVec3F> : maths::up_vector<LVec3F>), Up);
        Heading = NormalizeOr(Heading, AnyPerpendicular(Up));
    }
    return LookRotation(maths::normalize(Heading), Up);
}

//# Converts view-relative move axes (X right, Y up, Z forward) into body space.
NODISCARD constexpr LVec3F MoveAxesToBody(LVec3F const& Axes) noexcept
{
    return LVec3F{-Axes.x, Axes.y, Axes.z};
}

//# Converts body space into view-relative move axes (X right, Y up, Z forward).
NODISCARD constexpr LVec3F BodyToMoveAxes(LVec3F const& Body) noexcept
{
    return LVec3F{-Body.x, Body.y, Body.z};
}

//# The rotation of a look delta in body space. Yaw around up, then pitch around right, then roll around forward.
NODISCARD inline LWorldQuat LookDeltaToRotation(LVec3F const& Look) noexcept
{
    /* Right is -X in body space. */
    return maths::angle_axis(-Look.x, maths::up_vector<LVec3F>)
        * maths::angle_axis(-Look.y, maths::right_vector<LVec3F>)
        * maths::angle_axis(Look.z, maths::forward_vector<LVec3F>);
}

//# The local view rotation for a pitch in radians (positive looks up).
NODISCARD inline LWorldQuat PitchToRotation(f32 Pitch) noexcept
{
    return maths::angle_axis(-Pitch, maths::right_vector<LVec3F>);
}

} /* ~Namespace Movement */

} /* ~Namespace Jafg */
