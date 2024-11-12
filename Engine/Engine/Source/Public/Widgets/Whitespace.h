// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

struct ENGINE_API LWhitespace
{
    union
    {
        struct
        {
            float West;
            float North;
            float East;
            float South;
        };

        struct
        {
            float Left;
            float Top;
            float Right;
            float Bottom;
        };

        float Whitespaces[4];
    };

    FORCEINLINE LWhitespace()
        : West(0.0f)
        , North(0.0f)
        , East(0.0f)
        , South(0.0f)
    {
    }

    FORCEINLINE LWhitespace(const int32 UniformWhitespace)
        : West(static_cast<float>(UniformWhitespace))
        , North(static_cast<float>(UniformWhitespace))
        , East(static_cast<float>(UniformWhitespace))
        , South(static_cast<float>(UniformWhitespace))
    {
    }

    FORCEINLINE LWhitespace(const float UniformWhitespace)
        : West(UniformWhitespace)
        , North(UniformWhitespace)
        , East(UniformWhitespace)
        , South(UniformWhitespace)
    {
    }

    FORCEINLINE LWhitespace(const double UniformWhitespace)
        : West(static_cast<float>(UniformWhitespace))
        , North(static_cast<float>(UniformWhitespace))
        , East(static_cast<float>(UniformWhitespace))
        , South(static_cast<float>(UniformWhitespace))
    {
    }

    FORCEINLINE explicit LWhitespace(const int32 Horizontal, const int32 Vertical)
        : West(static_cast<float>(Horizontal))
        , North(static_cast<float>(Vertical))
        , East(static_cast<float>(Horizontal))
        , South(static_cast<float>(Vertical))
    {
    }

    FORCEINLINE explicit LWhitespace(const float Horizontal, const float Vertical)
        : West(Horizontal)
        , North(Vertical)
        , East(Horizontal)
        , South(Vertical)
    {
    }

    FORCEINLINE explicit LWhitespace(const double Horizontal, const double Vertical)
        : West(static_cast<float>(Horizontal))
        , North(static_cast<float>(Vertical))
        , East(static_cast<float>(Horizontal))
        , South(static_cast<float>(Vertical))
    {
    }

    FORCEINLINE explicit LWhitespace(const int32 InWest, const int32 InNorth, const int32 InEast, const int32 InSouth)
        : West(static_cast<float>(InWest))
        , North(static_cast<float>(InNorth))
        , East(static_cast<float>(InEast))
        , South(static_cast<float>(InSouth))
    {
    }

    FORCEINLINE explicit LWhitespace(const float InWest, const float InNorth, const float InEast, const float InSouth)
        : West(InWest)
        , North(InNorth)
        , East(InEast)
        , South(InSouth)
    {
    }

    FORCEINLINE explicit LWhitespace(const double InWest, const double InNorth, const double InEast, const double InSouth)
        : West(static_cast<float>(InWest))
        , North(static_cast<float>(InNorth))
        , East(static_cast<float>(InEast))
        , South(static_cast<float>(InSouth))
    {
    }

    FORCEINLINE LWhitespace operator*(const float Scalar) const
    {
        return LWhitespace(West * Scalar, North * Scalar, East * Scalar, South * Scalar);
    }

    FORCEINLINE LWhitespace operator*(const double Scalar) const
    {
        return LWhitespace(
            West * static_cast<float>(Scalar),
            North * static_cast<float>(Scalar),
            East * static_cast<float>(Scalar),
            South * static_cast<float>(Scalar)
        );
    }

    FORCEINLINE LWhitespace operator*(const LWhitespace& Other) const
    {
        return LWhitespace(West * Other.West, North * Other.North, East * Other.East, South * Other.South);
    }

    FORCEINLINE LWhitespace operator+(const LWhitespace& Other) const
    {
        return LWhitespace(West + Other.West, North + Other.North, East + Other.East, South + Other.South);
    }

    FORCEINLINE LWhitespace operator-(const LWhitespace& Other) const
    {
        return LWhitespace(West - Other.West, North - Other.North, East - Other.East, South - Other.South);
    }

    FORCEINLINE LWhitespace operator/(const float Scalar) const
    {
        const float InvScalar = 1.0f / Scalar;
        return LWhitespace(West * InvScalar, North * InvScalar, East * InvScalar, South * InvScalar);
    }

    FORCEINLINE LWhitespace operator/(const double Scalar) const
    {
        const float InvScalar = 1.0f / static_cast<float>(Scalar);
        return LWhitespace(West * InvScalar, North * InvScalar, East * InvScalar, South * InvScalar);
    }

    FORCEINLINE bool operator==(const LWhitespace& Other) const
    {
        return     Maths::IsNearlyEqual(West, Other.West)
                && Maths::IsNearlyEqual(North, Other.North)
                && Maths::IsNearlyEqual(East, Other.East)
                && Maths::IsNearlyEqual(South, Other.South);
    }

    FORCEINLINE bool operator!=(const LWhitespace& Other) const
    {
        return !(*this == Other);
    }

    FORCEINLINE LVector2 GetTopLeftOffset() const { return LVector2(Left, Top); }
    FORCEINLINE LVector2 GetDesiredSize()   const { return LVector2(Left + Right, Top + Bottom); }
};

typedef LWhitespace LPadding;
typedef LWhitespace LMargin;

} /* ~Namespace Jafg. */
