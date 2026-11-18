// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/EyeComponent.h"

namespace TestEye
{

//# A target that moves with a constant acceleration, sampled like the world does once per frame.
struct LMovingTarget final
{
    LWorldVec3 Translation{ maths::zero_vector<LWorldVec3> };
    LVec3F Velocity{ 0.0f };
    LVec3F Acceleration{ 0.0f };

    void Advance(f32 Dt) noexcept
    {
        this->Translation += LWorldVec3{this->Velocity * Dt + this->Acceleration * (0.5f * Dt * Dt)};
        this->Velocity += this->Acceleration * Dt;
    }
};

//# Simulates the spring for #Seconds at 60 frames per second.
inline void Simulate(Jafg::LEyeSpringState& State, Jafg::LEyeSpringSettings const& Settings, LMovingTarget& Target, LWorldQuat const& Rotation, f32 Seconds) noexcept
{
    constexpr f32 Dt{1.0f / 60.0f};
    for (f32 Time{0.0f}; Time < Seconds; Time += Dt)
    {
        Target.Advance(Dt);
        Jafg::StepEyeSpring(State, Settings, Rotation, Target.Velocity, Dt);
    }
}

//# Where the eye is relative to the target in the space of the target.
NODISCARD inline LVec3F LocalOffset(Jafg::LEyeSpringState const& State, LMovingTarget const&, LWorldQuat const& Rotation) noexcept
{
    return maths::inverse(Rotation) * State.Offset;
}

//# The angle of a rotation in radians.
NODISCARD inline f32 AngleOf(LWorldQuat const& Q) noexcept
{
    return 2.0f * std::acos(maths::clamp(std::abs(maths::normalize(Q).w), 0.0f, 1.0f));
}

} /* ~Namespace TestEye */

TRIVIAL_TEST_CASE("eye", SpringSettlesAtConstantVelocity)
{
    using namespace Jafg;
    using namespace TestEye;

    LEyeSpringSettings const Settings{.Frequency = 0.35f, .DampingRatio = 0.8f, .MaxOffset = LVec3F{3.0f, 3.0f, 6.0f}};
    LWorldQuat const Rotation{maths::identity<LWorldQuat>};

    /* Moving with the target from the start: Nothing happens. */
    {
        LMovingTarget Target{.Velocity = LVec3F{0.0f, 0.0f, 50.0f}};
        LEyeSpringState State{.Velocity = Target.Velocity};
        Simulate(State, Settings, Target, Rotation, 5.0f);
        TEST_LESS(maths::magnitude(LocalOffset(State, Target, Rotation)), 1.0e-2f)
    }

    /* Starting at rest behind a moving target: Catches up and settles at the target. */
    {
        LMovingTarget Target{.Velocity = LVec3F{10.0f, 0.0f, 50.0f}};
        LEyeSpringState State{};
        Simulate(State, Settings, Target, Rotation, 30.0f);
        TEST_LESS(maths::magnitude(LocalOffset(State, Target, Rotation)), 1.0e-2f)
        TEST_LESS(maths::magnitude(State.Velocity - Target.Velocity), 1.0e-2f)
    }
}

TRIVIAL_TEST_CASE("eye", SpringLagsUnderAcceleration)
{
    using namespace Jafg;
    using namespace TestEye;

    LEyeSpringSettings const Settings{.Frequency = 0.35f, .DampingRatio = 0.8f, .MaxOffset = LVec3F{100.0f}};
    f32 const Omega{2.0f * maths::pi_f * Settings.Frequency};
    LWorldQuat const Rotation{maths::identity<LWorldQuat>};

    LMovingTarget Target{.Acceleration = LVec3F{0.0f, 0.0f, 5.0f}};
    LEyeSpringState State{};
    Simulate(State, Settings, Target, Rotation, 20.0f);

    /*
     * A constant acceleration pushes the eye a / omega^2 against it. The world samples the velocity at the end of a
     * frame, which is half a frame ahead of the average velocity during the frame. That costs a few percent.
     */
    LVec3F const Offset{LocalOffset(State, Target, Rotation)};
    f32 const Expected{-5.0f / (Omega * Omega)};
    TEST_LESS(maths::abs(Offset.z - Expected), 0.06f * maths::abs(Expected))
    TEST_LESS(maths::abs(Offset.x) + maths::abs(Offset.y), 1.0e-3f)
}

TRIVIAL_TEST_CASE("eye", SpringStaysInsideMaximums)
{
    using namespace Jafg;
    using namespace TestEye;

    LEyeSpringSettings const Settings{.Frequency = 0.35f, .DampingRatio = 0.8f, .MaxOffset = LVec3F{3.0f, 2.0f, 6.0f}};
    /* The maximums are in the space of the target. */
    LWorldQuat const Rotation{maths::angle_axis(maths::half_pi_f, maths::up_vector<LVec3F>)};
    LVec3F const Forward{Rotation * maths::forward_vector<LVec3F>};
    LVec3F const Side{Rotation * maths::right_vector<LVec3F>};

    for (LVec3F const& Direction: {Forward, Side, maths::up_vector<LVec3F>, maths::normalize(Forward + Side)})
    {
        /* Far more than the maximums allow: 100 / omega^2 is about 20 meters. */
        LMovingTarget Target{.Acceleration = Direction * 100.0f};
        LEyeSpringState State{};
        f32 Worst{0.0f};
        for (i32 Chunk{0}; Chunk < 8; ++Chunk)
        {
            Simulate(State, Settings, Target, Rotation, 0.5f);
            Worst = maths::max(Worst, maths::magnitude(LocalOffset(State, Target, Rotation) / Settings.MaxOffset));
        }
        TEST_LESS(Worst, 1.0f + 1.0e-3f)
        /* At the border, behind the acceleration. */
        TEST_GREATER(Worst, 0.999f)
        TEST_LESS(maths::dot(State.Offset, Direction), 0.0f)

        /* Once the acceleration ends, it comes back instead of sticking to the border. */
        Target.Acceleration = LVec3F{0.0f};
        Simulate(State, Settings, Target, Rotation, 30.0f);
        TEST_LESS(maths::magnitude(LocalOffset(State, Target, Rotation)), 1.0e-2f)
    }
}

TRIVIAL_TEST_CASE("eye", Blend)
{
    using namespace Jafg;
    constexpr f32 Epsilon{1.0e-4f};

    LWorldEye const From{
        .vert_fov = 1.0f, .near_frustum = 0.1f, .far_frustum = 100.0f,
        .translation = LWorldVec3{0.0f},
        .front = maths::forward_vector<LWorldVec3>,
        .up = maths::up_vector<LWorldVec3>,
        };
    LWorldEye const To{
        .vert_fov = 2.0f, .near_frustum = 0.2f, .far_frustum = 200.0f,
        .translation = LWorldVec3{10.0f, 0.0f, 0.0f},
        .front = maths::right_vector<LWorldVec3>,
        .up = maths::up_vector<LWorldVec3>,
        };

    LWorldEye const Start{AEyeComponent::BlendEyes(From, To, 0.0f)};
    TEST_LESS(maths::magnitude(Start.translation - From.translation), Epsilon)
    TEST_LESS(maths::magnitude(Start.front - From.front), Epsilon)

    LWorldEye const End{AEyeComponent::BlendEyes(From, To, 1.0f)};
    TEST_LESS(maths::magnitude(End.translation - To.translation), Epsilon)
    TEST_LESS(maths::magnitude(End.front - To.front), Epsilon)
    TEST_LESS(maths::abs(End.vert_fov - To.vert_fov), Epsilon)

    LWorldEye const Middle{AEyeComponent::BlendEyes(From, To, 0.5f)};
    TEST_LESS(maths::magnitude(Middle.translation - LWorldVec3{5.0f, 0.0f, 0.0f}), Epsilon)
    TEST_LESS(maths::abs(maths::magnitude(Middle.front) - 1.0f), Epsilon)
    TEST_LESS(maths::abs(maths::dot(Middle.front, Middle.up)), Epsilon)
    /* Half way between forward and right. */
    TEST_LESS(maths::magnitude(Middle.front - LWorldVec3{maths::normalize(LVec3F{1.0f, 0.0f, 1.0f})}), 1.0e-3f)
    TEST_LESS(maths::abs(Middle.vert_fov - 1.5f), Epsilon)

    /* The rotation of an eye reproduces its basis. */
    LWorldQuat const Rotation{AEyeComponent::RotationOf(To)};
    TEST_LESS(maths::magnitude(Rotation * maths::forward_vector<LVec3F> - LVec3F{To.front}), Epsilon)
    TEST_LESS(maths::magnitude(Rotation * maths::up_vector<LVec3F> - LVec3F{To.up}), Epsilon)
}

TRIVIAL_TEST_CASE("eye", RotationLag)
{
    using namespace Jafg;
    using TestEye::AngleOf;
    constexpr f32 Dt{1.0f / 60.0f};

    /* A constant turn makes it lag about rate / speed behind, against the turn. */
    {
        LVec3F const Turn{maths::up_vector<LVec3F> * 1.0f};
        LWorldQuat Lag{maths::identity<LWorldQuat>};
        for (i32 Frame{0}; Frame < 600; ++Frame)
        {
            StepEyeRotationLag(Lag, Turn, 5.0f, maths::pi_f, Dt);
        }
        TEST_LESS(maths::abs(AngleOf(Lag) - 1.0f / 5.0f), 0.015f)
        TEST_LESS(maths::dot(LVec3F{Lag.x, Lag.y, Lag.z}, Turn), 0.0f)

        /* And catches up once the turn ends. */
        for (i32 Frame{0}; Frame < 300; ++Frame)
        {
            StepEyeRotationLag(Lag, LVec3F{0.0f}, 5.0f, maths::pi_f, Dt);
        }
        TEST_LESS(AngleOf(Lag), 1.0e-3f)
    }

    /* Never more than the maximum. */
    {
        f32 const Max{maths::radians(12.0f)};
        LWorldQuat Lag{maths::identity<LWorldQuat>};
        f32 Worst{0.0f};
        for (i32 Frame{0}; Frame < 600; ++Frame)
        {
            StepEyeRotationLag(Lag, maths::normalize(LVec3F{1.0f, 2.0f, 0.5f}) * 10.0f, 1.0f, Max, Dt);
            Worst = maths::max(Worst, AngleOf(Lag));
        }
        TEST_LESS(Worst, Max + 1.0e-3f)
        TEST_GREATER(Worst, Max - 1.0e-3f)
    }
}
