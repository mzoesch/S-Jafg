// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/InterfaceTypes.h"

#define GET_IN_SPT(Ret, Function) \
    FORCEINLINE constexpr Ret LAL_JOIN_INNER_TWO(Function, InSpt)() const noexcept \
    { \
    if (this->Type == EWidgetSize::StaticPoints) \
    { \
    return this->Function().Size; \
    } \
    check( this->Type == EWidgetSize::Points ) \
    return ToStaticPoints(this->Function()); \
    }

namespace Jafg
{

ENGINE_API f32 ToStaticPoints(LWidgetSize1 Size) noexcept;
ENGINE_API LVector2 ToStaticPoints(LWidgetSize2 Size) noexcept;

struct LWhitespace
{
    EWidgetSize::Type Type{ EWidgetSize::Points };

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

    FORCEINLINE constexpr LWhitespace(const EWidgetSize::Type InType, const i32 UniformWhitespace) noexcept
        : Type(InType)
        , West(static_cast<f32>(UniformWhitespace))
        , North(static_cast<f32>(UniformWhitespace))
        , East(static_cast<f32>(UniformWhitespace))
        , South(static_cast<f32>(UniformWhitespace))
    {
    }

    FORCEINLINE constexpr LWhitespace(const EWidgetSize::Type InType, const f32 UniformWhitespace) noexcept
        : Type(InType)
        , West(UniformWhitespace)
        , North(UniformWhitespace)
        , East(UniformWhitespace)
        , South(UniformWhitespace)
    {
    }

    FORCEINLINE constexpr LWhitespace(const EWidgetSize::Type InType, const f64 UniformWhitespace) noexcept
        : Type(InType)
        , West(static_cast<f32>(UniformWhitespace))
        , North(static_cast<f32>(UniformWhitespace))
        , East(static_cast<f32>(UniformWhitespace))
        , South(static_cast<f32>(UniformWhitespace))
    {
    }

    FORCEINLINE constexpr LWhitespace(const EWidgetSize::Type InType, const i32 Horizontal, const i32 Vertical) noexcept
        : Type(InType)
        , West(static_cast<f32>(Horizontal))
        , North(static_cast<f32>(Vertical))
        , East(static_cast<f32>(Horizontal))
        , South(static_cast<f32>(Vertical))
    {
    }

    FORCEINLINE constexpr LWhitespace(const EWidgetSize::Type InType, const f32 Horizontal, const f32 Vertical) noexcept
        : Type(InType)
        , West(Horizontal)
        , North(Vertical)
        , East(Horizontal)
        , South(Vertical)
    {
    }

    FORCEINLINE constexpr LWhitespace(const EWidgetSize::Type InType, const f64 Horizontal, const f64 Vertical) noexcept
        : Type(InType)
        , West(static_cast<f32>(Horizontal))
        , North(static_cast<f32>(Vertical))
        , East(static_cast<f32>(Horizontal))
        , South(static_cast<f32>(Vertical))
    {
    }

    FORCEINLINE constexpr LWhitespace(const EWidgetSize::Type InType, const i32 InWest, const i32 InNorth, const i32 InEast, const i32 InSouth) noexcept
        : Type(InType)
        , West(static_cast<f32>(InWest))
        , North(static_cast<f32>(InNorth))
        , East(static_cast<f32>(InEast))
        , South(static_cast<f32>(InSouth))
    {
    }

    FORCEINLINE constexpr LWhitespace(const EWidgetSize::Type InType, const f32 InWest, const f32 InNorth, const f32 InEast, const f32 InSouth) noexcept
        : Type(InType)
        , West(InWest)
        , North(InNorth)
        , East(InEast)
        , South(InSouth)
    {
    }

    FORCEINLINE constexpr LWhitespace(const EWidgetSize::Type InType, const f64 InWest, const f64 InNorth, const f64 InEast, const f64 InSouth) noexcept
        : Type(InType)
        , West(static_cast<f32>(InWest))
        , North(static_cast<f32>(InNorth))
        , East(static_cast<f32>(InEast))
        , South(static_cast<f32>(InSouth))
    {
    }

    FORCEINLINE constexpr LWhitespace operator*(const f32 Scalar) const noexcept
    {
        return LWhitespace{this->Type, this->West * Scalar, this->North * Scalar, this->East * Scalar, this->South * Scalar};
    }

    FORCEINLINE constexpr LWhitespace operator*(const f64 Scalar) const noexcept
    {
        return LWhitespace{
            this->Type,
            this->West * static_cast<f32>(Scalar),
            this->North * static_cast<f32>(Scalar),
            this->East * static_cast<f32>(Scalar),
            this->South * static_cast<f32>(Scalar)
        };
    }

    FORCEINLINE constexpr LWhitespace operator*(const LWhitespace& Other) const noexcept
    {
        return LWhitespace{this->Type, this->West * Other.West, this->North * Other.North, this->East * Other.East, this->South * Other.South};
    }

    FORCEINLINE constexpr LWhitespace operator+(const LWhitespace& Other) const noexcept
    {
        return LWhitespace{this->Type, this->West + Other.West, this->North + Other.North, this->East + Other.East, this->South + Other.South};
    }

    FORCEINLINE constexpr LWhitespace operator-(const LWhitespace& Other) const noexcept
    {
        return LWhitespace{this->Type, this->West - Other.West, this->North - Other.North, this->East - Other.East, this->South - Other.South};
    }

    FORCEINLINE constexpr LWhitespace operator/(const f32 Scalar) const noexcept
    {
        const f32 InvScalar{ 1.0f / Scalar };
        return LWhitespace{this->Type, this->West * InvScalar, this->North * InvScalar, this->East * InvScalar, this->South * InvScalar};
    }

    FORCEINLINE constexpr LWhitespace operator/(const f64 Scalar) const noexcept
    {
        const f32 InvScalar{ 1.0f / static_cast<f32>(Scalar) };
        return {this->Type, this->West * InvScalar, this->North * InvScalar, this->East * InvScalar, this->South * InvScalar};
    }

    FORCEINLINE constexpr bool operator==(const LWhitespace& Other) const noexcept
    {
        return Type == Other.Type
            && Maths::IsNearlyEqual(West, Other.West)
            && Maths::IsNearlyEqual(North, Other.North)
            && Maths::IsNearlyEqual(East, Other.East)
            && Maths::IsNearlyEqual(South, Other.South);
    }

    FORCEINLINE constexpr bool operator!=(const LWhitespace& Other) const noexcept
    {
        return !(*this == Other);
    }

    FORCEINLINE constexpr LWidgetSize1 GetLeftOffset() const noexcept { return {this->Type, this->Left}; }
    FORCEINLINE constexpr LWidgetSize1 GetTopOffset() const noexcept { return {this->Type, this->Top}; }
    FORCEINLINE constexpr LWidgetSize2 GetTopLeftOffset() const noexcept { return {this->Type, this->Left, this->Top}; }
    FORCEINLINE constexpr LWidgetSize1 GetRightOffset() const noexcept { return {this->Type, this->Right}; }
    FORCEINLINE constexpr LWidgetSize1 GetBottomOffset() const noexcept { return {this->Type, this->Bottom}; }
    FORCEINLINE constexpr LWidgetSize2 GetBottomRightOffset() const noexcept { return {this->Type, this->Right, this->Bottom}; }
    FORCEINLINE constexpr LWidgetSize1 GetDesiredSizeX() const noexcept { return {this->Type, this->Left + this->Right}; }
    FORCEINLINE constexpr LWidgetSize1 GetDesiredSizeY() const noexcept { return {this->Type, this->Top + this->Bottom}; }
    FORCEINLINE constexpr LWidgetSize2 GetDesiredSize() const noexcept { return {this->Type, this->Left + this->Right, this->Top + this->Bottom}; }

    GET_IN_SPT(f32, GetLeftOffset)
    GET_IN_SPT(f32, GetTopOffset)
    GET_IN_SPT(LVector2, GetTopLeftOffset)
    GET_IN_SPT(f32, GetRightOffset)
    GET_IN_SPT(f32, GetBottomOffset)
    GET_IN_SPT(LVector2, GetBottomRightOffset)
    GET_IN_SPT(f32, GetDesiredSizeX)
    GET_IN_SPT(f32, GetDesiredSizeY)
    GET_IN_SPT(LVector2, GetDesiredSize)
};

typedef LWhitespace LPadding;
typedef LWhitespace LMargin;

} /* ~Namespace Jafg */

#undef GET_IN_SPT
