// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Frustum.h"

namespace TestFrustum
{

NODISCARD inline LWorldEye MakeEye() noexcept
{
    LWorldQuat const Rotation{maths::normalize(
          maths::angle_axis(0.7f, maths::up_vector<LVec3F>)
        * maths::angle_axis(-0.3f, maths::right_vector<LVec3F>)
        )};
    return LWorldEye{
        .vert_fov = maths::pi_f / 3.0f, .near_frustum = 0.5f, .far_frustum = 100.0f,
        .translation = LWorldVec3{3.0f, -2.0f, 5.0f},
        .front = Rotation * maths::forward_vector<LWorldVec3>,
        .up = Rotation * maths::up_vector<LWorldVec3>,
        };
}

//# Signed distance to the plane. Positive towards its normal.
NODISCARD inline LWorldReal Distance(LWorldPlane3 const& Plane, LWorldVec3 const& Point) noexcept
{
    return maths::dot(Point - Plane.origin, Plane.normal);
}

} /* ~Namespace TestFrustum */

TRIVIAL_TEST_CASE("frustum", CornersMatchProjection)
{
    using namespace Jafg;
    using namespace TestFrustum;

    LWorldEye const Eye{MakeEye()};
    f32 const Aspect{16.0f / 9.0f};
    LFrustum const Frustum{LFrustum::FromEye(Eye, Aspect)};

    /* What the world draws with (without the flip of the device convention). */
    LMat4F const ViewProj{
          maths::perspective<LWorldReal,world_qual>(Eye.vert_fov, Aspect, Eye.near_frustum, Eye.far_frustum)
        * maths::look_at(Eye.translation, Eye.translation + Eye.front, Eye.up)
        };

    for (std::size_t Idx{0}; Idx < LFrustum::CornerCount; ++Idx)
    {
        LVec4F Clip{ViewProj * LVec4F{Frustum.Corners[Idx], 1.0f}};
        LVec3F const Ndc{LVec3F{Clip} / Clip.w};
        TEST_LESS(maths::abs(Ndc.x - ((Idx & 0b001) ? 1.0f : -1.0f)), 1.0e-3f)
        TEST_LESS(maths::abs(Ndc.y - ((Idx & 0b010) ? 1.0f : -1.0f)), 1.0e-3f)
        TEST_LESS(maths::abs(Ndc.z - ((Idx & 0b100) ? 1.0f : 0.0f)), 1.0e-3f)
    }
}

TRIVIAL_TEST_CASE("frustum", PlanesBoundTheVolume)
{
    using namespace Jafg;
    using namespace TestFrustum;

    LWorldEye const Eye{MakeEye()};
    LFrustum const Frustum{LFrustum::FromEye(Eye, 16.0f / 9.0f)};
    constexpr LWorldReal Epsilon{1.0e-3f};

    for (auto const& Plane: Frustum.Planes)
    {
        TEST_LESS(maths::abs(maths::magnitude(Plane.normal) - 1.0f), Epsilon)
    }

    /* Every corner lies on the three planes that it belongs to and inside the other three. */
    for (std::size_t Idx{0}; Idx < LFrustum::CornerCount; ++Idx)
    {
        std::array<LFrustum::EPlane, 3> const On{
            (Idx & 0b001) ? LFrustum::Right : LFrustum::Left,
            (Idx & 0b010) ? LFrustum::Top : LFrustum::Bottom,
            (Idx & 0b100) ? LFrustum::Far : LFrustum::Near,
            };
        for (u8 Plane{0}; Plane < LFrustum::PlaneCount; ++Plane)
        {
            LWorldReal const D{Distance(Frustum.Planes[Plane], Frustum.Corners[Idx])};
            if (algo::contains(On, static_cast<LFrustum::EPlane>(Plane)))
            {
                TEST_LESS(maths::abs(D), Epsilon * Eye.far_frustum)
            }
            else
            {
                TEST_GREATER(D, -Epsilon * Eye.far_frustum)
            }
        }
    }

    auto IsInside{[&Frustum](LWorldVec3 const& Point)
    {
        return algo::all_of(Frustum.Planes, [&Point](LWorldPlane3 const& Plane){ return Distance(Plane, Point) >= 0.0f; });
    }};
    LWorldVec3 const Front{maths::normalize(Eye.front)};
    LWorldVec3 const Right{maths::normalize(maths::cross(Front, Eye.up))};
    TEST_TRUE(IsInside(Eye.translation + Front * 10.0f))
    TEST_TRUE(IsInside(Eye.translation + Front * 10.0f + Right * 5.0f))
    /* Before the near plane, behind the far plane, behind the eye and too far to the side. */
    TEST_FALSE(IsInside(Eye.translation + Front * 0.25f))
    TEST_FALSE(IsInside(Eye.translation + Front * 101.0f))
    TEST_FALSE(IsInside(Eye.translation - Front * 10.0f))
    TEST_FALSE(IsInside(Eye.translation + Front * 10.0f + Right * 50.0f))

    /* Twelve edges, each between two different corners. */
    for (LWorldLine3 const& Edge: Frustum.GetEdges())
    {
        TEST_GREATER(maths::magnitude(Edge.end - Edge.begin), Epsilon)
    }
}
