// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

enum struct EWidgetSize
{
    StaticPoints,
    Points,
};

struct LWidgetSize1 final
{
    EWidgetSize Type{ EWidgetSize::Points };
    f32 Size;

    FORCEINLINE constexpr LWidgetSize1() noexcept : Size(0.0) { }
    FORCEINLINE constexpr LWidgetSize1(EWidgetSize Type, f32 Size) noexcept
        : Type(Type), Size(Size)
    {
        return;
    }

    FORCEINLINE constexpr LWidgetSize1(const LWidgetSize1& WidgetSize1) noexcept
    {
        this->Type = WidgetSize1.Type;
        this->Size = WidgetSize1.Size;
        return;
    }

    FORCEINLINE constexpr LWidgetSize1& operator=(const LWidgetSize1& WidgetSize1) noexcept
    {
        this->Type = WidgetSize1.Type;
        this->Size = WidgetSize1.Size;
        return *this;
    }

    FORCEINLINE constexpr LWidgetSize1(LWidgetSize1&& WidgetSize1) noexcept
    {
        this->Type = WidgetSize1.Type;
        this->Size = std::move(WidgetSize1.Size);
        return;
    }

    FORCEINLINE constexpr LWidgetSize1& operator=(LWidgetSize1&& WidgetSize1) noexcept
    {
        this->Type = WidgetSize1.Type;
        this->Size = std::move(WidgetSize1.Size);
        return *this;
    }

    FORCEINLINE constexpr LWidgetSize1 operator*(const f32 Scalar) const noexcept
    {
        return LWidgetSize1{ this->Type, this->Size * Scalar };
    }

    FORCEINLINE static constexpr LWidgetSize1 Zero() noexcept
    {
        return LWidgetSize1{ EWidgetSize::Points, 0.0f };
    }
};

struct LWidgetSize2 final
{
    EWidgetSize Type{ EWidgetSize::Points };

    union
    {
        struct
        {
            f32 X;
            f32 Y;
        };

        LVec2F Size;
    };

    FORCEINLINE constexpr LWidgetSize2() noexceptcheck : Size{} { check( this->X == 0.0 && this->Y == 0.0 ) }
    FORCEINLINE constexpr LWidgetSize2(LWidgetSize2 const& WidgetSize2) noexcept
    {
        this->Type = WidgetSize2.Type;
        this->Size = WidgetSize2.Size;
        return;
    }
    FORCEINLINE constexpr LWidgetSize2& operator=(LWidgetSize2 const& WidgetSize2) noexcept
    {
        this->Type = WidgetSize2.Type;
        this->Size = WidgetSize2.Size;
        return *this;
    }
    FORCEINLINE constexpr LWidgetSize2(LWidgetSize2&& WidgetSize2) noexcept
    {
        this->Type = WidgetSize2.Type;
        this->Size = std::move(WidgetSize2.Size);
        return;
    }
    FORCEINLINE constexpr LWidgetSize2& operator=(LWidgetSize2&& WidgetSize2) noexcept
    {
        this->Type = WidgetSize2.Type;
        this->Size = std::move(WidgetSize2.Size);
        return *this;
    }

    FORCEINLINE constexpr LWidgetSize2(EWidgetSize Type, f32 X, f32 Y) noexcept
        : Type(Type), Size(X, Y)
    {
        return;
    }

    FORCEINLINE constexpr LWidgetSize2(EWidgetSize Type, LVec2D Size) noexcept
        : Type(Type), Size(Size)
    {
        return;
    }

    FORCEINLINE constexpr LWidgetSize2(LWidgetSize1 X, f32 Y) noexcept
        : Type(X.Type), Size(X.Size, Y)
    {
        return;
    }

    FORCEINLINE constexpr void SetXAxis(LWidgetSize1 InSize) noexcept
    {
        check( this->Size.y == 0.0 )

        this->Type = InSize.Type;
        this->Size.x = InSize.Size;

        return;
    }

    FORCEINLINE constexpr void SetYAxis(LWidgetSize1 InSize) noexcept
    {
        check( this->Size.x == 0.0 )

        this->Type = InSize.Type;
        this->Size.y = InSize.Size;

        return;
    }

    FORCEINLINE constexpr LWidgetSize2 operator*(const f32 Scalar) const noexcept
    {
        return LWidgetSize2{ this->Type, this->Size.x * Scalar, this->Size.y * Scalar };
    }

    FORCEINLINE static constexpr LWidgetSize2 Zero() noexcept
    {
        return LWidgetSize2{ EWidgetSize::Points, 0.0, 0.0 };
    }
};

} /* ~Namespace Jafg. */

FORCEINLINE constexpr Jafg::LWidgetSize1 operator ""_spt(unsigned long long Value)
{
    return Jafg::LWidgetSize1{ Jafg::EWidgetSize::StaticPoints, static_cast<f32>(Value) };
}

FORCEINLINE constexpr Jafg::LWidgetSize1 operator ""_spt(long double Value)
{
    return Jafg::LWidgetSize1{ Jafg::EWidgetSize::StaticPoints, static_cast<f32>(Value) };
}

FORCEINLINE constexpr Jafg::LWidgetSize1 operator ""_pt(unsigned long long Value)
{
    return Jafg::LWidgetSize1{ Jafg::EWidgetSize::Points, static_cast<f32>(Value) };
}

FORCEINLINE constexpr Jafg::LWidgetSize1 operator ""_pt(long double Value)
{
    return Jafg::LWidgetSize1{ Jafg::EWidgetSize::Points, static_cast<f32>(Value) };
}

FORCEINLINE constexpr Jafg::LWidgetSize2 operator ""_spt2(unsigned long long Value)
{
    return Jafg::LWidgetSize2{ Jafg::EWidgetSize::StaticPoints, static_cast<f32>(Value), static_cast<f32>(Value) };
}

FORCEINLINE constexpr Jafg::LWidgetSize2 operator ""_spt2(long double Value)
{
    return Jafg::LWidgetSize2{ Jafg::EWidgetSize::StaticPoints, static_cast<f32>(Value), static_cast<f32>(Value) };
}

FORCEINLINE constexpr Jafg::LWidgetSize2 operator ""_pt2(unsigned long long Value)
{
    return Jafg::LWidgetSize2{ Jafg::EWidgetSize::Points, static_cast<f32>(Value), static_cast<f32>(Value) };
}

FORCEINLINE constexpr Jafg::LWidgetSize2 operator ""_pt2(long double Value)
{
    return Jafg::LWidgetSize2{ Jafg::EWidgetSize::Points, static_cast<f32>(Value), static_cast<f32>(Value) };
}
