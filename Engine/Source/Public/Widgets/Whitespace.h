// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/InterfaceTypes.h"
#include "User/UserPreferencesForward.h"

#define PRIVATE_JAFG_GET_IN_SPT(Ret, Function)                                     \
    FORCEINLINE constexpr Ret JAFG_JOIN_INNER_TWO(Function, InSpt)                  \
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

ENGINE_API  f32 InSpt(LViewport const& Viewport, LWidgetSize1 Size) noexcept;
FORCEINLINE f32 InSpt(WNode const& Node, LWidgetSize1 Size) noexcept;
ENGINE_API  LVec2F InSpt(LViewport const& Viewport, LWidgetSize2 Size) noexcept;
FORCEINLINE LVec2F InSpt(WNode const& Node, LWidgetSize2 Size) noexcept;

ENGINE_API  f32 InSptFromRelative(LViewport const& Viewport, f32 Relative) noexcept;
FORCEINLINE f32 InSptFromRelative(WNode const& Node, f32 Relative) noexcept;
ENGINE_API  LVec2F InSptFromRelative(LViewport const& Viewport, LVec2F Relative) noexcept;
FORCEINLINE LVec2F InSptFromRelative(WNode const& Node, LVec2F Relative) noexcept;

struct LWhitespace
{
    EWidgetSize Type{ EWidgetSize::Points };

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

    FORCEINLINE constexpr LWhitespace(const f32 UniformWhitespace) noexcept
        : West(UniformWhitespace)
        , North(UniformWhitespace)
        , East(UniformWhitespace)
        , South(UniformWhitespace)
    {
    }

    FORCEINLINE constexpr LWhitespace(const f32 Horizontal, const f32 Vertical) noexcept
        : West(Horizontal)
        , North(Vertical)
        , East(Horizontal)
        , South(Vertical)
    {
    }

    FORCEINLINE constexpr LWhitespace(const f32 InWest, const f32 InNorth, const f32 InEast, const f32 InSouth) noexcept
        : West(InWest)
        , North(InNorth)
        , East(InEast)
        , South(InSouth)
    {
    }

    FORCEINLINE constexpr LWhitespace(const LVec2F& WestNorth, f32 InEast, f32 InSouth) noexcept
        : West(WestNorth.x)
        , North(WestNorth.y)
        , East(InEast)
        , South(InSouth)
    {
    }

    FORCEINLINE constexpr LWhitespace(const EWidgetSize InType, const f32 UniformWhitespace) noexcept
        : Type(InType)
        , West(UniformWhitespace)
        , North(UniformWhitespace)
        , East(UniformWhitespace)
        , South(UniformWhitespace)
    {
    }

    FORCEINLINE constexpr LWhitespace(const EWidgetSize InType, const f32 Horizontal, const f32 Vertical) noexcept
        : Type(InType)
        , West(Horizontal)
        , North(Vertical)
        , East(Horizontal)
        , South(Vertical)
    {
    }

    FORCEINLINE constexpr LWhitespace(const EWidgetSize InType, const f32 InWest, const f32 InNorth, const f32 InEast, const f32 InSouth) noexcept
        : Type(InType)
        , West(InWest)
        , North(InNorth)
        , East(InEast)
        , South(InSouth)
    {
    }

    FORCEINLINE constexpr LWhitespace(const EWidgetSize InType, const LVec2F& WestNorth, f32 InEast, f32 InSouth) noexcept
        : Type(InType)
        , West(WestNorth.x)
        , North(WestNorth.y)
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

    FORCEINLINE constexpr LWhitespace(const LWidgetSize1 InWest, const f32 InNorth, const f32 InEast, const f32 InSouth) noexcept
        : Type(InWest.Type)
        , West(InWest.Size)
        , North(InNorth)
        , East(InEast)
        , South(InSouth)
    {
    }

    FORCEINLINE constexpr LWhitespace(const LWidgetSize1 Horizontal, const f32 Vertical) noexcept
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

    FORCEINLINE constexpr LWhitespace operator*(const f32 Scalar) const noexcept
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

    FORCEINLINE constexpr LWhitespace operator/(const f32 Scalar) const noexcept
    {
        const f32 InvScalar{ 1.0f / static_cast<f32>(Scalar) };
        return {this->Type, this->West * InvScalar, this->North * InvScalar, this->East * InvScalar, this->South * InvScalar};
    }

    FORCEINLINE constexpr bool operator==(const LWhitespace& Other) const noexcept
    {
        return Type == Other.Type
            && maths::eq_e(West, Other.West)
            && maths::eq_e(North, Other.North)
            && maths::eq_e(East, Other.East)
            && maths::eq_e(South, Other.South);
    }

    FORCEINLINE constexpr f32 GetLeftOffsetRaw() const noexceptcheck { check( this->Type == EWidgetSize::StaticPoints ) return this->Left; }
    FORCEINLINE constexpr f32 GetTopOffsetRaw() const noexceptcheck { check( this->Type == EWidgetSize::StaticPoints ) return this->Top; }
    FORCEINLINE constexpr LVec2F GetTopLeftOffsetRaw() const noexceptcheck { check( this->Type == EWidgetSize::StaticPoints ) return {this->Left, this->Top}; }
    FORCEINLINE constexpr f32 GetRightOffsetRaw() const noexceptcheck { check( this->Type == EWidgetSize::StaticPoints ) return this->Right; }
    FORCEINLINE constexpr f32 GetBottomOffsetRaw() const noexceptcheck { check( this->Type == EWidgetSize::StaticPoints ) return this->Bottom; }
    FORCEINLINE constexpr LVec2F GetBottomRightOffsetRaw() const noexceptcheck { check( this->Type == EWidgetSize::StaticPoints ) return {this->Right, this->Bottom}; }
    FORCEINLINE constexpr f32 GetDesiredSizeXRaw() const noexceptcheck { check( this->Type == EWidgetSize::StaticPoints ) return this->Left + this->Right; }
    FORCEINLINE constexpr f32 GetDesiredSizeYRaw() const noexceptcheck { check( this->Type == EWidgetSize::StaticPoints ) return this->Top + this->Bottom; }
    FORCEINLINE constexpr LVec2F GetDesiredSizeRaw() const noexceptcheck { check( this->Type == EWidgetSize::StaticPoints ) return {this->Left + this->Right, this->Top + this->Bottom}; }

    FORCEINLINE constexpr LWidgetSize1 GetLeftOffset() const noexcept { return {this->Type, this->Left}; }
    FORCEINLINE constexpr LWidgetSize1 GetTopOffset() const noexcept { return {this->Type, this->Top}; }
    FORCEINLINE constexpr LWidgetSize2 GetTopLeftOffset() const noexcept { return {this->Type, this->Left, this->Top}; }
    FORCEINLINE constexpr LWidgetSize1 GetRightOffset() const noexcept { return {this->Type, this->Right}; }
    FORCEINLINE constexpr LWidgetSize1 GetBottomOffset() const noexcept { return {this->Type, this->Bottom}; }
    FORCEINLINE constexpr LWidgetSize2 GetBottomRightOffset() const noexcept { return {this->Type, this->Right, this->Bottom}; }
    FORCEINLINE constexpr LWidgetSize1 GetDesiredSizeX() const noexcept { return {this->Type, this->Left + this->Right}; }
    FORCEINLINE constexpr LWidgetSize1 GetDesiredSizeY() const noexcept { return {this->Type, this->Top + this->Bottom}; }
    FORCEINLINE constexpr LWidgetSize2 GetDesiredSize() const noexcept { return {this->Type, this->Left + this->Right, this->Top + this->Bottom}; }

    PRIVATE_JAFG_GET_IN_SPT(f32, GetLeftOffset)
    PRIVATE_JAFG_GET_IN_SPT(f32, GetTopOffset)
    PRIVATE_JAFG_GET_IN_SPT(LVec2F, GetTopLeftOffset)
    PRIVATE_JAFG_GET_IN_SPT(f32, GetRightOffset)
    PRIVATE_JAFG_GET_IN_SPT(f32, GetBottomOffset)
    PRIVATE_JAFG_GET_IN_SPT(LVec2F, GetBottomRightOffset)
    PRIVATE_JAFG_GET_IN_SPT(f32, GetDesiredSizeX)
    PRIVATE_JAFG_GET_IN_SPT(f32, GetDesiredSizeY)
    PRIVATE_JAFG_GET_IN_SPT(LVec2F, GetDesiredSize)

    FORCEINLINE LWhitespace InSpt(auto& Context) const noexcept
    {
        if (this->Type == EWidgetSize::StaticPoints)
        {
            return *this;
        }

        check( this->Type == EWidgetSize::Points )
        LVec2F Vertical{ InSptFromRelative(Context, LVec2F{ this->North, this->South }) };
        LVec2F Horizontal{ InSptFromRelative(Context, LVec2F{ this->West, this->East }) };

        return LWhitespace
        {
            EWidgetSize::StaticPoints,
            Horizontal.x, Vertical.x,
            Horizontal.y, Vertical.y
        };
    }
};

typedef LWhitespace LPadding;
typedef LWhitespace LMargin;

} /* ~Namespace Jafg */

#undef PRIVATE_JAFG_GET_IN_SPT
