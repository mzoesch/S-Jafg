// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

//#
//# The volume that an eye sees in world space. Bounded by six planes, the near and the far plane of the eye included.
//# It matches what the world draws for an eye (#maths::look_at and #maths::perspective).
//#
struct LFrustum final
{
    //# Left, right, bottom and top as seen through the eye.
    enum EPlane : u8
    {
        Left, Right, Bottom, Top, Near, Far,

        PlaneCount,
    };

    static constexpr std::size_t CornerCount{8};
    static constexpr std::size_t EdgeCount{12};

    //# Where the eye was. The four side planes meet here.
    LWorldVec3 Origin;
    //# The normal of every plane is normalized and points into the frustum.
    std::array<LWorldPlane3, PlaneCount> Planes;
    //#
    //# Bit 0 is set for the right (else left), bit 1 for the top (else bottom) and bit 2 for the far (else near) corners.
    //# E.g., #Corners[0b101] is the bottom right corner of the far plane.
    //#
    std::array<LWorldVec3, CornerCount> Corners;

    //# @param Aspect Width divided by height of the target that the eye is drawn into.
    NODISCARD static LFrustum FromEye(LWorldEye const& Eye, f32 Aspect) noexcept
    {
        check(Aspect > 0.0f)
        check(Eye.near_frustum > 0.0_r && Eye.far_frustum > Eye.near_frustum)

        //# The same basis as #maths::look_at.
        LWorldVec3 const ViewFront{maths::normalize(Eye.front)};
        LWorldVec3 const ViewRight{maths::normalize(maths::cross(ViewFront, Eye.up))};
        LWorldVec3 const ViewUp{maths::cross(ViewRight, ViewFront)};

        LWorldReal const TanHalfHeight{static_cast<LWorldReal>(maths::tan(Eye.vert_fov * 0.5_r))};
        LWorldReal const TanHalfWidth{TanHalfHeight * static_cast<LWorldReal>(Aspect)};

        LFrustum Frustum;
        Frustum.Origin = Eye.translation;

        for (std::size_t Idx{0}; Idx < CornerCount; ++Idx)
        {
            LWorldReal const Depth{(Idx & 0b100) ? Eye.far_frustum : Eye.near_frustum};
            LWorldReal const X{(Idx & 0b001) ? 1.0_r : -1.0_r};
            LWorldReal const Y{(Idx & 0b010) ? 1.0_r : -1.0_r};
            Frustum.Corners[Idx] = Eye.translation
                + ViewFront * Depth
                + ViewRight * (X * Depth * TanHalfWidth)
                + ViewUp * (Y * Depth * TanHalfHeight);
        }

        Frustum.Planes[Left]   = {.origin = Eye.translation, .normal = maths::normalize(ViewFront * TanHalfWidth + ViewRight)};
        Frustum.Planes[Right]  = {.origin = Eye.translation, .normal = maths::normalize(ViewFront * TanHalfWidth - ViewRight)};
        Frustum.Planes[Bottom] = {.origin = Eye.translation, .normal = maths::normalize(ViewFront * TanHalfHeight + ViewUp)};
        Frustum.Planes[Top]    = {.origin = Eye.translation, .normal = maths::normalize(ViewFront * TanHalfHeight - ViewUp)};
        Frustum.Planes[Near]   = {.origin = Eye.translation + ViewFront * Eye.near_frustum, .normal = ViewFront};
        Frustum.Planes[Far]    = {.origin = Eye.translation + ViewFront * Eye.far_frustum, .normal = -ViewFront};

        return Frustum;
    }

    //# The edges of the near plane, then the ones of the far plane and then the ones that connect both.
    NODISCARD constexpr std::array<LWorldLine3, EdgeCount> GetEdges() const noexcept
    {
        auto const& C{this->Corners};
        return {{
            {C[0b000], C[0b001]}, {C[0b001], C[0b011]}, {C[0b011], C[0b010]}, {C[0b010], C[0b000]},
            {C[0b100], C[0b101]}, {C[0b101], C[0b111]}, {C[0b111], C[0b110]}, {C[0b110], C[0b100]},
            {C[0b000], C[0b100]}, {C[0b001], C[0b101]}, {C[0b010], C[0b110]}, {C[0b011], C[0b111]},
            }};
    }
};

} /* ~Namespace Jafg */
