// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

//# The type of lights Jafg supports (stable across slang).
enum struct ELight: u32
{
    Directional,
    Point,
    Spot,
    Rect,
};

//# Each light has a unit of measurement for its intensity.
enum struct ELightUnits: u8
{
    //# Auto.
    Identity,

    //#
    //# Used by: ELight::Directional
    //#
    Lux,

    //#
    //# Candela: ...
    //#
    //# Used by: ELight::Point, ELight::Spot
    //#
    Cd,

    //# Used by: ELight::Rect
    Luminance,
};

enum struct ECastRayTraceShadows: u8
{
    Never,
    User,
    Always,
};

struct LLight final
{
    LWorldVec3 Translation{ maths::zero_vector<LWorldVec3> };
    LWorldQuat Rotation{ maths::identity<LWorldQuat> };

    //# The type of light.
    ELight Type{ ELight::Directional };
    //# The unit of measurement for the light's intensity.
    NODISCARD FORCEINLINE constexpr ELightUnits GetUnits() const noexcept
    {
        switch (this->Type)
        {
        case ELight::Directional: return ELightUnits::Lux;
        case ELight::Point:       return ELightUnits::Cd;
        case ELight::Spot:        return ELightUnits::Cd;
        case ELight::Rect:        return ELightUnits::Luminance;
        }
        return ELightUnits::Identity;
    }

    //#
    //# Whether this light can ever affect the world. If false, then this light is forever disabled (as it is not
    //# registered to the world's light subsystem) and any modifications to it result are voided.
    //#
    bool bCanEverAffectWorld:1{ true };

    //# Whether this light is enabled.
    bool bEnabled:1{ true };

    //# Whether this light is allowed to cast any shadows.
    bool bCastShadows:1{};
    //# Whether this light can cast shadows for static objects. Requires #bCastShadows.
    bool bCastStaticShadows:1{};
    //# Whether this light can cast shadows for dynamic objects. Requires #bCastShadows.
    bool bCastDynamicShadows:1{};

    //#
    //# - Directional: Direction the light travels
    //# - Point:       Unused
    //# - Spot:        Direction the cone points to
    //# - Rect:        Emitting normal of the front face
    //#
    LVec3F Direction;

    //# Tint of the light.
    LLinearColor Tint{ LinearColors::White };

};

} /* ~Namespace Jafg */
