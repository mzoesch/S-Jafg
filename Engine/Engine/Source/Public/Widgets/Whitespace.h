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
            f32 West;
            f32 North;
            f32 East;
            f32 South;
        };

        struct
        {
            f32 Left;
            f32 Top;
            f32 Right;
            f32 Bottom;
        };

        f32 Whitespaces[4];
    };

    FORCEINLINE constexpr LWhitespace() noexcept
        : West(0.0f)
        , North(0.0f)
        , East(0.0f)
        , South(0.0f)
    {
    }

    FORCEINLINE constexpr LWhitespace(const i32 UniformWhitespace) noexcept
        : West(static_cast<f32>(UniformWhitespace))
        , North(static_cast<f32>(UniformWhitespace))
        , East(static_cast<f32>(UniformWhitespace))
        , South(static_cast<f32>(UniformWhitespace))
    {
    }

    FORCEINLINE constexpr LWhitespace(const f32 UniformWhitespace) noexcept
        : West(UniformWhitespace)
        , North(UniformWhitespace)
        , East(UniformWhitespace)
        , South(UniformWhitespace)
    {
    }

    FORCEINLINE constexpr LWhitespace(const f64 UniformWhitespace) noexcept
        : West(static_cast<f32>(UniformWhitespace))
        , North(static_cast<f32>(UniformWhitespace))
        , East(static_cast<f32>(UniformWhitespace))
        , South(static_cast<f32>(UniformWhitespace))
    {
    }

    FORCEINLINE constexpr LWhitespace(const i32 Horizontal, const i32 Vertical) noexcept
        : West(static_cast<f32>(Horizontal))
        , North(static_cast<f32>(Vertical))
        , East(static_cast<f32>(Horizontal))
        , South(static_cast<f32>(Vertical))
    {
    }

    FORCEINLINE constexpr LWhitespace(const f32 Horizontal, const f32 Vertical) noexcept
        : West(Horizontal)
        , North(Vertical)
        , East(Horizontal)
        , South(Vertical)
    {
    }

    FORCEINLINE constexpr LWhitespace(const f64 Horizontal, const f64 Vertical) noexcept
        : West(static_cast<f32>(Horizontal))
        , North(static_cast<f32>(Vertical))
        , East(static_cast<f32>(Horizontal))
        , South(static_cast<f32>(Vertical))
    {
    }

    FORCEINLINE constexpr LWhitespace(const i32 InWest, const i32 InNorth, const i32 InEast, const i32 InSouth) noexcept
        : West(static_cast<f32>(InWest))
        , North(static_cast<f32>(InNorth))
        , East(static_cast<f32>(InEast))
        , South(static_cast<f32>(InSouth))
    {
    }

    FORCEINLINE constexpr LWhitespace(const f32 InWest, const f32 InNorth, const f32 InEast, const f32 InSouth) noexcept
        : West(InWest)
        , North(InNorth)
        , East(InEast)
        , South(InSouth)
    {
    }

    FORCEINLINE constexpr LWhitespace(const f64 InWest, const f64 InNorth, const f64 InEast, const f64 InSouth) noexcept
        : West(static_cast<f32>(InWest))
        , North(static_cast<f32>(InNorth))
        , East(static_cast<f32>(InEast))
        , South(static_cast<f32>(InSouth))
    {
    }

    FORCEINLINE constexpr LWhitespace operator*(const f32 Scalar) const noexcept
    {
        return LWhitespace(West * Scalar, North * Scalar, East * Scalar, South * Scalar);
    }

    FORCEINLINE constexpr LWhitespace operator*(const f64 Scalar) const noexcept
    {
        return LWhitespace(
            West * static_cast<f32>(Scalar),
            North * static_cast<f32>(Scalar),
            East * static_cast<f32>(Scalar),
            South * static_cast<f32>(Scalar)
        );
    }

    FORCEINLINE constexpr LWhitespace operator*(const LWhitespace& Other) const noexcept
    {
        return LWhitespace(West * Other.West, North * Other.North, East * Other.East, South * Other.South);
    }

    FORCEINLINE constexpr LWhitespace operator+(const LWhitespace& Other) const noexcept
    {
        return LWhitespace(West + Other.West, North + Other.North, East + Other.East, South + Other.South);
    }

    FORCEINLINE constexpr LWhitespace operator-(const LWhitespace& Other) const noexcept
    {
        return LWhitespace(West - Other.West, North - Other.North, East - Other.East, South - Other.South);
    }

    FORCEINLINE constexpr LWhitespace operator/(const f32 Scalar) const noexcept
    {
        const f32 InvScalar = 1.0f / Scalar;
        return LWhitespace(West * InvScalar, North * InvScalar, East * InvScalar, South * InvScalar);
    }

    FORCEINLINE constexpr LWhitespace operator/(const f64 Scalar) const noexcept
    {
        const f32 InvScalar = 1.0f / static_cast<f32>(Scalar);
        return {this->West * InvScalar, this->North * InvScalar, this->East * InvScalar, this->South * InvScalar};
    }

    FORCEINLINE constexpr bool operator==(const LWhitespace& Other) const noexcept
    {
        return Maths::IsNearlyEqual(West, Other.West)
            && Maths::IsNearlyEqual(North, Other.North)
            && Maths::IsNearlyEqual(East, Other.East)
            && Maths::IsNearlyEqual(South, Other.South);
    }

    FORCEINLINE constexpr bool operator!=(const LWhitespace& Other) const noexcept
    {
        return !(*this == Other);
    }

    FORCEINLINE constexpr f32      GetLeftOffset() const noexcept { return this->Left; }
    FORCEINLINE constexpr f32      GetTopOffset() const noexcept { return this->Top; }
    FORCEINLINE constexpr LVector2 GetTopLeftOffset() const noexcept { return {this->Left, this->Top}; }
    FORCEINLINE constexpr f32      GetRightOffset() const noexcept { return this->Right; }
    FORCEINLINE constexpr f32      GetBottomOffset() const noexcept { return this->Bottom; }
    FORCEINLINE constexpr LVector2 GetBottomRightOffset() const noexcept { return {this->Right, this->Bottom}; }
    FORCEINLINE constexpr f32      GetDesiredSizeX() const noexcept { return this->Left + this->Right; }
    FORCEINLINE constexpr f32      GetDesiredSizeY() const noexcept { return this->Top + this->Bottom; }
    FORCEINLINE constexpr LVector2 GetDesiredSize() const noexcept { return {this->Left + this->Right, this->Top + this->Bottom}; }
};

typedef LWhitespace LPadding;
typedef LWhitespace LMargin;

} /* ~Namespace Jafg */
