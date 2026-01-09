// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/InterfaceTypes.h"
#include "User/UserPreferencesForward.h"

#define PRIVATE_JAFG_GET_IN_SPT(Ret, Function)                                     \
    FORCEINLINE constexpr Ret LAL_JOIN_INNER_TWO(Function, InSpt)                  \
        (auto const& Context) const noexcept                                       \
    {                                                                              \
        if (this->Type == EWidgetSize::StaticPoints)                               \
        {                                                                          \
            return this->Function().Size;                                          \
        }                                                                          \
        check( this->Type == EWidgetSize::Points )                                 \
        return Jafg::InSpt(Context, this->Function());                             \
    }

namespace Jafg
{

class LViewport;
class WNode;

ENGINE_API  f64 InSpt(LViewport const& Viewport, LWidgetSize1 Size) noexcept;
FORCEINLINE f64 InSpt(WNode const& Node, LWidgetSize1 Size) noexcept;
ENGINE_API  LVector2D InSpt(LViewport const& Viewport, LWidgetSize2 Size) noexcept;
FORCEINLINE LVector2D InSpt(WNode const& Node, LWidgetSize2 Size) noexcept;

ENGINE_API  f64 InSptFromRelative(LViewport const& Viewport, f64 Relative) noexcept;
FORCEINLINE f64 InSptFromRelative(WNode const& Node, f64 Relative) noexcept;
ENGINE_API  LVector2D InSptFromRelative(LViewport const& Viewport, LVector2D Relative) noexcept;
FORCEINLINE LVector2D InSptFromRelative(WNode const& Node, LVector2D Relative) noexcept;

struct LWhitespace
{
    EWidgetSize::Type Type{ EWidgetSize::Points };

    union
    {
        struct
        {
            f64 West;
            f64 North;
            f64 East;
            f64 South;
        };

        struct
        {
            f64 Left;
            f64 Top;
            f64 Right;
            f64 Bottom;
        };

        f64 Whitespaces[4];
    };

    FORCEINLINE constexpr LWhitespace() noexcept
        : West(0.0f)
        , North(0.0f)
        , East(0.0f)
        , South(0.0f)
    {
    }

    FORCEINLINE constexpr LWhitespace(const f64 UniformWhitespace) noexcept
        : West(UniformWhitespace)
        , North(UniformWhitespace)
        , East(UniformWhitespace)
        , South(UniformWhitespace)
    {
    }

    FORCEINLINE constexpr LWhitespace(const f64 Horizontal, const f64 Vertical) noexcept
        : West(Horizontal)
        , North(Vertical)
        , East(Horizontal)
        , South(Vertical)
    {
    }

    FORCEINLINE constexpr LWhitespace(const f64 InWest, const f64 InNorth, const f64 InEast, const f64 InSouth) noexcept
        : West(InWest)
        , North(InNorth)
        , East(InEast)
        , South(InSouth)
    {
    }

    FORCEINLINE constexpr LWhitespace(const LVector2D& WestNorth, f64 InEast, f64 InSouth) noexcept
        : West(WestNorth.X)
        , North(WestNorth.Y)
        , East(InEast)
        , South(InSouth)
    {
    }

    FORCEINLINE constexpr LWhitespace(const EWidgetSize::Type InType, const f64 UniformWhitespace) noexcept
        : Type(InType)
        , West(UniformWhitespace)
        , North(UniformWhitespace)
        , East(UniformWhitespace)
        , South(UniformWhitespace)
    {
    }

    FORCEINLINE constexpr LWhitespace(const EWidgetSize::Type InType, const f64 Horizontal, const f64 Vertical) noexcept
        : Type(InType)
        , West(Horizontal)
        , North(Vertical)
        , East(Horizontal)
        , South(Vertical)
    {
    }

    FORCEINLINE constexpr LWhitespace(const EWidgetSize::Type InType, const f64 InWest, const f64 InNorth, const f64 InEast, const f64 InSouth) noexcept
        : Type(InType)
        , West(InWest)
        , North(InNorth)
        , East(InEast)
        , South(InSouth)
    {
    }

    FORCEINLINE constexpr LWhitespace(const EWidgetSize::Type InType, const LVector2D& WestNorth, f64 InEast, f64 InSouth) noexcept
        : Type(InType)
        , West(WestNorth.X)
        , North(WestNorth.Y)
        , East(InEast)
        , South(InSouth)
    {
    }

    FORCEINLINE constexpr LWhitespace(const LWidgetSize1 Size) noexcept
        : Type(Size.Type)
        , West(Size.Size)
        , North(Size.Size)
        , East(Size.Size)
        , South(Size.Size)
    {
    }

    FORCEINLINE constexpr LWhitespace(const LWidgetSize1 InWest, const f64 InNorth, const f64 InEast, const f64 InSouth) noexcept
        : Type(InWest.Type)
        , West(InWest.Size)
        , North(InNorth)
        , East(InEast)
        , South(InSouth)
    {
    }

    FORCEINLINE constexpr LWhitespace(const LWidgetSize1 Horizontal, const f64 Vertical) noexcept
        : Type(Horizontal.Type)
        , West(Horizontal.Size)
        , North(Vertical)
        , East(Horizontal.Size)
        , South(Vertical)
    {
    }

    FORCEINLINE constexpr LWhitespace(const LWidgetSize2 Size) noexcept
        : Type(Size.Type)
        , West(Size.X)
        , North(Size.Y)
        , East(Size.X)
        , South(Size.Y)
    {
    }

    FORCEINLINE constexpr LWhitespace operator*(const f64 Scalar) const noexcept
    {
        return LWhitespace{
            this->Type, this->West * Scalar, this->North * Scalar, this->East * Scalar, this->South * Scalar
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

    FORCEINLINE constexpr LWhitespace operator/(const f64 Scalar) const noexcept
    {
        const f64 InvScalar{ 1.0 / static_cast<f64>(Scalar) };
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

    FORCEINLINE constexpr f64 GetLeftOffsetRaw() const noexceptcheck { check( this->Type == EWidgetSize::StaticPoints ) return this->Left; }
    FORCEINLINE constexpr f64 GetTopOffsetRaw() const noexceptcheck { check( this->Type == EWidgetSize::StaticPoints ) return this->Top; }
    FORCEINLINE constexpr LVector2D GetTopLeftOffsetRaw() const noexceptcheck { check( this->Type == EWidgetSize::StaticPoints ) return {this->Left, this->Top}; }
    FORCEINLINE constexpr f64 GetRightOffsetRaw() const noexceptcheck { check( this->Type == EWidgetSize::StaticPoints ) return this->Right; }
    FORCEINLINE constexpr f64 GetBottomOffsetRaw() const noexceptcheck { check( this->Type == EWidgetSize::StaticPoints ) return this->Bottom; }
    FORCEINLINE constexpr LVector2D GetBottomRightOffsetRaw() const noexceptcheck { check( this->Type == EWidgetSize::StaticPoints ) return {this->Right, this->Bottom}; }
    FORCEINLINE constexpr f64 GetDesiredSizeXRaw() const noexceptcheck { check( this->Type == EWidgetSize::StaticPoints ) return this->Left + this->Right; }
    FORCEINLINE constexpr f64 GetDesiredSizeYRaw() const noexceptcheck { check( this->Type == EWidgetSize::StaticPoints ) return this->Top + this->Bottom; }
    FORCEINLINE constexpr LVector2D GetDesiredSizeRaw() const noexceptcheck { check( this->Type == EWidgetSize::StaticPoints ) return {this->Left + this->Right, this->Top + this->Bottom}; }

    FORCEINLINE constexpr LWidgetSize1 GetLeftOffset() const noexcept { return {this->Type, this->Left}; }
    FORCEINLINE constexpr LWidgetSize1 GetTopOffset() const noexcept { return {this->Type, this->Top}; }
    FORCEINLINE constexpr LWidgetSize2 GetTopLeftOffset() const noexcept { return {this->Type, this->Left, this->Top}; }
    FORCEINLINE constexpr LWidgetSize1 GetRightOffset() const noexcept { return {this->Type, this->Right}; }
    FORCEINLINE constexpr LWidgetSize1 GetBottomOffset() const noexcept { return {this->Type, this->Bottom}; }
    FORCEINLINE constexpr LWidgetSize2 GetBottomRightOffset() const noexcept { return {this->Type, this->Right, this->Bottom}; }
    FORCEINLINE constexpr LWidgetSize1 GetDesiredSizeX() const noexcept { return {this->Type, this->Left + this->Right}; }
    FORCEINLINE constexpr LWidgetSize1 GetDesiredSizeY() const noexcept { return {this->Type, this->Top + this->Bottom}; }
    FORCEINLINE constexpr LWidgetSize2 GetDesiredSize() const noexcept { return {this->Type, this->Left + this->Right, this->Top + this->Bottom}; }

    PRIVATE_JAFG_GET_IN_SPT(f64, GetLeftOffset)
    PRIVATE_JAFG_GET_IN_SPT(f64, GetTopOffset)
    PRIVATE_JAFG_GET_IN_SPT(LVector2D, GetTopLeftOffset)
    PRIVATE_JAFG_GET_IN_SPT(f64, GetRightOffset)
    PRIVATE_JAFG_GET_IN_SPT(f64, GetBottomOffset)
    PRIVATE_JAFG_GET_IN_SPT(LVector2D, GetBottomRightOffset)
    PRIVATE_JAFG_GET_IN_SPT(f64, GetDesiredSizeX)
    PRIVATE_JAFG_GET_IN_SPT(f64, GetDesiredSizeY)
    PRIVATE_JAFG_GET_IN_SPT(LVector2D, GetDesiredSize)

    FORCEINLINE LWhitespace InSpt(auto& Context) const noexcept
    {
        if (this->Type == EWidgetSize::StaticPoints)
        {
            return *this;
        }

        check( this->Type == EWidgetSize::Points )
        LVector2D Vertical{ InSptFromRelative(Context, LVector2D{ this->North, this->South }) };
        LVector2D Horizontal{ InSptFromRelative(Context, LVector2D{ this->West, this->East }) };

        return LWhitespace
        {
            EWidgetSize::StaticPoints,
            Horizontal.X, Vertical.X,
            Horizontal.Y, Vertical.Y
        };
    }
};

typedef LWhitespace LPadding;
typedef LWhitespace LMargin;

} /* ~Namespace Jafg */

#undef PRIVATE_JAFG_GET_IN_SPT
