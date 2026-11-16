// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

//# The type of lights Jafg supports (stable across slang).
DECLARE_TRIVIAL_SERDE_ENUM(ELight, u32,
    Directional,
    Point,
    Spot,
    Rect
    )

//# Each light has a unit of measurement for its intensity.
enum struct ELightUnits: u8
{
    //# Auto.
    Identity,

    //#
    //# Lux (lm/m^2): The illuminance that arrives at a surface that directly faces the light.
    //#
    //# Used by: ELight::Directional
    //#
    Lux,

    //#
    //# Candela (lm/sr): The luminous intensity the light emits in a direction. The illuminance that arrives at a
    //# surface falls off with the inverse square of the distance. Not affected by the cone angles of a spot light.
    //#
    //# Used by: ELight::Point, ELight::Spot
    //#
    Cd,

    //#
    //# Nits (cd/m^2): The luminance of the emitting surface. A bigger surface therefore emits more light.
    //#
    //# Used by: ELight::Rect
    //#
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
    //# The orientation of the light. A light points along its forward axis, see #GetDirection.
    LWorldQuat Rotation{ maths::identity<LWorldQuat> };

    //#
    //# The normalized direction this light points to:
    //# - Directional: Direction the light travels
    //# - Point:       Unused
    //# - Spot:        Direction the cone points to
    //# - Rect:        Emitting normal of the front face
    //#
    NODISCARD FORCEINLINE LVec3F GetDirection() const noexcept
    {
        return maths::normalize(this->Rotation * maths::forward_vector<LVec3F>);
    }
    //# The normalized axis along the #Width of a rect light. Unused by all other types.
    NODISCARD FORCEINLINE LVec3F GetRight() const noexcept
    {
        return maths::normalize(this->Rotation * maths::right_vector<LVec3F>);
    }
    //# The normalized axis along the #Height of a rect light. Unused by all other types.
    NODISCARD FORCEINLINE LVec3F GetUp() const noexcept
    {
        return maths::normalize(this->Rotation * maths::up_vector<LVec3F>);
    }

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
    bool bCanEverAffectWorld{ true };

    //# Whether this light is enabled.
    bool bEnabled{ true };

    //# Whether this light is allowed to cast any shadows. Not a bit field, so that the editor can bind to it.
    bool bCastShadows{};

    //# Tint of the light.
    LLinearColor3 Tint{ LinearColors3::White };

    //#
    //# How bright the light is, in the unit of its type (see #GetUnits). A light without intensity is not rendered.
    //#
    //# Defaults to pi: A directional light with that illuminance lights a white diffuse surface that directly faces
    //# it exactly white.
    //#
    f32 Intensity{ maths::pi_f };

    //#
    //# Multiplier for the specular highlights this light causes, with zero disabling them entirely.
    //# Anything but one is not physically based.
    //#
    f32 SpecularScale{ 1.0f };

    //#
    //# The distance in meters at which the light is smoothly cut off. Zero means that the light reaches infinitely
    //# far. This is not the brightness of the light: Inside of its range, a light always falls off physically based.
    //#
    //# Used by: ELight::Point, ELight::Spot, ELight::Rect
    //#
    f32 Range{ 8.0f };

    //#
    //# The radius in meters of the sphere that emits the light. Zero is an infinitely small (punctual) source.
    //# The bigger the source, the bigger and softer its specular highlights. It does not change how much light is
    //# emitted.
    //#
    //# Used by: ELight::Point, ELight::Spot
    //#
    f32 SourceRadius{};

    //#
    //# The angular diameter in radians of the disc that emits the light, as seen from the lit surface. Zero is an
    //# infinitely small (punctual) source. The sun covers roughly 0.0093 rad (0.53 deg).
    //#
    //# Used by: ELight::Directional
    //#
    f32 SourceAngle{};

    //#
    //# The angle in radians between the axis of the cone and where the light starts to fade out.
    //# Clamped to #OuterConeAngle.
    //#
    //# Used by: ELight::Spot
    //#
    f32 InnerConeAngle{};

    //#
    //# The angle in radians between the axis of the cone and its edge, where the light is faded out entirely.
    //# Range [0,pi].
    //#
    //# Used by: ELight::Spot
    //#
    f32 OuterConeAngle{ maths::pi_f * 0.25f };

    //#
    //# The size in meters of the emitting rectangle along its right axis (see #GetRight).
    //#
    //# Used by: ELight::Rect
    //#
    f32 Width{ 1.0f };

    //#
    //# The size in meters of the emitting rectangle along its up axis (see #GetUp).
    //#
    //# Used by: ELight::Rect
    //#
    f32 Height{ 1.0f };
};

} /* ~Namespace Jafg */
