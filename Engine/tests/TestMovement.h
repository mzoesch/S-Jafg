// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/MovementTypes.h"

namespace TestMovement
{

NODISCARD inline f32 Distance(LVec3F const& A, LVec3F const& B) noexcept
{
    return maths::magnitude(A - B);
}

//# The visual right of an unrotated body.
inline constexpr LVec3F BodyRight{maths::cross(maths::forward_vector<LVec3F>, maths::up_vector<LVec3F>)};

} /* ~Namespace TestMovement */

TRIVIAL_TEST_CASE("movement", VectorHelpers)
{
    using namespace Jafg;
    using TestMovement::Distance;
    constexpr f32 Epsilon{1.0e-4f};

    TEST_LESS(Distance(Movement::ProjectOnPlane(LVec3F{1.0f, 2.0f, 3.0f}, maths::up_vector<LVec3F>), LVec3F{1.0f, 0.0f, 3.0f}), Epsilon)

    TEST_LESS(maths::abs(maths::magnitude(Movement::ClampMagnitude(LVec3F{3.0f, 4.0f, 0.0f}, 1.0f)) - 1.0f), Epsilon)
    TEST_LESS(Distance(Movement::ClampMagnitude(LVec3F{0.3f, 0.4f, 0.0f}, 1.0f), LVec3F{0.3f, 0.4f, 0.0f}), Epsilon)

    TEST_LESS(Distance(Movement::NormalizeOr(LVec3F{0.0f}, maths::up_vector<LVec3F>), maths::up_vector<LVec3F>), Epsilon)
    TEST_LESS(Distance(Movement::NormalizeOr(LVec3F{0.0f, 0.0f, 5.0f}, maths::up_vector<LVec3F>), maths::forward_vector<LVec3F>), Epsilon)

    TEST_LESS(Distance(Movement::MoveTowards(LVec3F{0.0f}, LVec3F{10.0f, 0.0f, 0.0f}, 1.0f), LVec3F{1.0f, 0.0f, 0.0f}), Epsilon)
    TEST_LESS(Distance(Movement::MoveTowards(LVec3F{0.0f}, LVec3F{0.5f, 0.0f, 0.0f}, 1.0f), LVec3F{0.5f, 0.0f, 0.0f}), Epsilon)

    for (LVec3F const& V: {maths::up_vector<LVec3F>, maths::forward_vector<LVec3F>, maths::right_vector<LVec3F>, maths::normalize(LVec3F{1.0f, 2.0f, 3.0f})})
    {
        LVec3F const P{Movement::AnyPerpendicular(V)};
        TEST_LESS(maths::abs(maths::dot(P, V)), Epsilon)
        TEST_LESS(maths::abs(maths::magnitude(P) - 1.0f), Epsilon)
    }
}

TRIVIAL_TEST_CASE("movement", Acceleration)
{
    using namespace Jafg;
    using TestMovement::Distance;
    constexpr f32 Epsilon{1.0e-4f};
    LVec3F const X{1.0f, 0.0f, 0.0f};

    /* Accelerates towards the wish speed. */
    TEST_LESS(Distance(Movement::Accelerate(LVec3F{0.0f}, X, 5.0f, 10.0f, 0.1f), LVec3F{1.0f, 0.0f, 0.0f}), Epsilon)
    /* Never beyond the wish speed. */
    TEST_LESS(Distance(Movement::Accelerate(LVec3F{4.5f, 0.0f, 0.0f}, X, 5.0f, 10.0f, 0.1f), LVec3F{5.0f, 0.0f, 0.0f}), Epsilon)
    /* Faster than the wish speed is kept (momentum). */
    TEST_LESS(Distance(Movement::Accelerate(LVec3F{8.0f, 0.0f, 0.0f}, X, 5.0f, 10.0f, 0.1f), LVec3F{8.0f, 0.0f, 0.0f}), Epsilon)
    /* Perpendicular motion is untouched. */
    TEST_LESS(Distance(Movement::Accelerate(LVec3F{0.0f, 3.0f, 0.0f}, X, 5.0f, 10.0f, 0.1f), LVec3F{1.0f, 3.0f, 0.0f}), Epsilon)

    /* Dampening without thrust brakes everything. */
    TEST_LESS(Distance(Movement::Dampen(LVec3F{0.0f, -1.0f, 0.0f}, LVec3F{0.0f}, 10.0f, 0.1f), LVec3F{0.0f}), Epsilon)
    /* Dampening keeps the motion along the thrust and brakes the rest. */
    TEST_LESS(Distance(Movement::Dampen(LVec3F{2.0f, 1.0f, 0.0f}, X, 5.0f, 0.1f), LVec3F{2.0f, 0.5f, 0.0f}), Epsilon)
    /* Motion against the thrust is braked. */
    TEST_LESS(Distance(Movement::Dampen(LVec3F{-2.0f, 0.0f, 0.0f}, X, 5.0f, 0.1f), LVec3F{-1.5f, 0.0f, 0.0f}), Epsilon)

    /* Dampeners that exceed the gravity hover. */
    LVec3F Velocity{0.0f};
    LVec3F const Gravity{0.0f, -9.81f, 0.0f};
    for (int Tick{}; Tick < 120; ++Tick)
    {
        Velocity = Movement::Dampen(Velocity + Gravity / 60.0f, LVec3F{0.0f}, 15.0f, 1.0f / 60.0f);
    }
    TEST_LESS(maths::magnitude(Velocity), Epsilon)
}

TRIVIAL_TEST_CASE("movement", Rotations)
{
    using namespace Jafg;
    using TestMovement::Distance;
    using TestMovement::BodyRight;
    constexpr f32 Epsilon{1.0e-3f};
    LVec3F const Up{maths::up_vector<LVec3F>};
    LVec3F const Forward{maths::forward_vector<LVec3F>};

    TEST_LESS(Distance(Movement::FromToRotation(Up, Forward) * Up, Forward), Epsilon)
    TEST_LESS(Distance(Movement::FromToRotation(Up, -Up) * Up, -Up), Epsilon)
    TEST_LESS(Distance(Movement::FromToRotation(Up, Up) * Forward, Forward), Epsilon)

    LVec3F const Rotated{Movement::RotateTowards(Up, Forward, 0.1f)};
    TEST_LESS(maths::abs(maths::acos(maths::clamp(maths::dot(Rotated, Up), -1.0f, 1.0f)) - 0.1f), Epsilon)
    TEST_LESS(Distance(Movement::RotateTowards(Up, Forward, 10.0f), Forward), Epsilon)
    TEST_LESS(maths::abs(maths::magnitude(Movement::RotateTowards(Up, -Up, 0.5f)) - 1.0f), Epsilon)

    /* Twist around up of a pure yaw is the yaw itself. A pure pitch has no twist around up. */
    LWorldQuat const Yaw{maths::angle_axis(0.5f, Up)};
    TEST_GREATER(maths::abs(glm::dot(Movement::ExtractTwist(Yaw, Up), Yaw)), 1.0f - Epsilon)
    LWorldQuat const Pitch{maths::angle_axis(0.5f, BodyRight)};
    TEST_GREATER(maths::abs(glm::dot(Movement::ExtractTwist(Pitch, Up), maths::identity<LWorldQuat>)), 1.0f - Epsilon)

    TEST_GREATER(maths::abs(glm::dot(Movement::LookRotation(Forward, Up), maths::identity<LWorldQuat>)), 1.0f - Epsilon)
    TEST_LESS(Distance(Movement::LookRotation(BodyRight, Up) * Forward, BodyRight), Epsilon)
    TEST_LESS(Distance(Movement::LookRotation(BodyRight, Up) * Up, Up), Epsilon)

    /* Upright keeps the heading and the up. */
    LWorldQuat const Upright{Movement::MakeUpright(Yaw * maths::angle_axis(0.7f, BodyRight), Up)};
    TEST_LESS(Distance(Upright * Up, Up), Epsilon)
    TEST_LESS(Distance(Upright * Forward, Yaw * Forward), Epsilon)
    /* Even when looking straight down. */
    LWorldQuat const Down{Movement::MakeUpright(maths::angle_axis(-maths::half_pi_f, BodyRight), Up)};
    TEST_LESS(Distance(Down * Up, Up), Epsilon)
}

TRIVIAL_TEST_CASE("movement", Conventions)
{
    using namespace Jafg;
    using TestMovement::Distance;
    using TestMovement::BodyRight;
    constexpr f32 Epsilon{1.0e-4f};
    LVec3F const Up{maths::up_vector<LVec3F>};
    LVec3F const Forward{maths::forward_vector<LVec3F>};

    /* Move axes: X right, Y up, Z forward. */
    TEST_LESS(Distance(Movement::MoveAxesToBody(LVec3F{1.0f, 0.0f, 0.0f}), BodyRight), Epsilon)
    TEST_LESS(Distance(Movement::MoveAxesToBody(LVec3F{0.0f, 1.0f, 0.0f}), Up), Epsilon)
    TEST_LESS(Distance(Movement::MoveAxesToBody(LVec3F{0.0f, 0.0f, 1.0f}), Forward), Epsilon)
    TEST_LESS(Distance(Movement::BodyToMoveAxes(Movement::MoveAxesToBody(LVec3F{0.3f, -0.2f, 0.7f})), LVec3F{0.3f, -0.2f, 0.7f}), Epsilon)

    /* Look: positive yaw turns right, positive pitch looks up, positive roll rolls right. */
    TEST_GREATER(maths::dot(Movement::LookDeltaToRotation(LVec3F{0.1f, 0.0f, 0.0f}) * Forward, BodyRight), 0.0f)
    TEST_GREATER(maths::dot(Movement::LookDeltaToRotation(LVec3F{0.0f, 0.1f, 0.0f}) * Forward, Up), 0.0f)
    TEST_GREATER(maths::dot(Movement::LookDeltaToRotation(LVec3F{0.0f, 0.0f, 0.1f}) * Up, BodyRight), 0.0f)
    TEST_LESS(Distance(Movement::PitchToRotation(0.2f) * Forward, Movement::LookDeltaToRotation(LVec3F{0.0f, 0.2f, 0.0f}) * Forward), Epsilon)

    /* The default intent of the controller is that dampeners and magnetic boots are on. */
    LMovementInput Input;
    Input.Buttons = EMovementButtonBits::Dampeners | EMovementButtonBits::MagBoots;
    TEST_TRUE(Input.Has(EMovementButtonBits::Dampeners))
    TEST_FALSE(Input.Has(EMovementButtonBits::Jetpack))

    TEST_TRUE(IsUprightMovementMode(EMovementMode::Walking))
    TEST_TRUE(IsUprightMovementMode(EMovementMode::MagWalking))
    TEST_FALSE(IsUprightMovementMode(EMovementMode::ZeroG))
    TEST_TRUE(IsGroundedMovementMode(EMovementMode::MagWalking))
    TEST_FALSE(IsGroundedMovementMode(EMovementMode::Flying))
}

TRIVIAL_TEST_CASE("movement", AirControl)
{
    using namespace Jafg;
    constexpr f32 Epsilon{1.0e-6f};

    /* Vacuum: Nothing to push against. */
    TEST_LESS(maths::abs(Movement::AirControlFactor(0.0f, 1.225f)), Epsilon)
    /* Proportional below the reference, complete at and above it. */
    TEST_LESS(maths::abs(Movement::AirControlFactor(0.6125f, 1.225f) - 0.5f), Epsilon)
    TEST_LESS(maths::abs(Movement::AirControlFactor(1.225f, 1.225f) - 1.0f), Epsilon)
    TEST_LESS(maths::abs(Movement::AirControlFactor(10.0f, 1.225f) - 1.0f), Epsilon)
    /* Without a reference, any air is enough. */
    TEST_LESS(maths::abs(Movement::AirControlFactor(0.1f, 0.0f) - 1.0f), Epsilon)
    TEST_LESS(maths::abs(Movement::AirControlFactor(0.0f, 0.0f)), Epsilon)
}
