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

    FORCEINLINE constexpr LWidgetSize1() noexcept : Size(0.0) {}
    FORCEINLINE constexpr LWidgetSize1(EWidgetSize Type, f32 Size) noexcept : Type(Type), Size(Size) {}
    FORCEINLINE constexpr LWidgetSize1(LWidgetSize1 const& WidgetSize1) noexcept
    {
        this->Type = WidgetSize1.Type;
        this->Size = WidgetSize1.Size;
        return;
    }
    FORCEINLINE constexpr LWidgetSize1& operator=(LWidgetSize1 const& WidgetSize1) noexcept
    {
        this->Type = WidgetSize1.Type;
        this->Size = WidgetSize1.Size;
        return *this;
    }

    FORCEINLINE constexpr bool operator==(LWidgetSize1 const& Other) const noexcept
    {
        return this->Type == Other.Type && this->Size == Other.Size;
    }
    FORCEINLINE constexpr LWidgetSize1 operator*(f32 Scalar) const noexcept
    {
        return LWidgetSize1{this->Type, this->Size * Scalar};
    }
};

struct LWidgetSize2 final
{
    EWidgetSize Type{ EWidgetSize::Points };
    union
    {
        struct{ f32 X; f32 Y; };
        LVec2F Size;
    };

    FORCEINLINE constexpr LWidgetSize2() noexcept : Size{maths::zero_vector<LVec2F>} {}
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
    FORCEINLINE constexpr LWidgetSize2(EWidgetSize Type, f32 X, f32 Y) noexcept : Type{Type}, X{X}, Y{Y} {}
    FORCEINLINE constexpr LWidgetSize2(EWidgetSize Type, LVec2F Size) noexcept : Type{Type}, Size{Size} {}
    FORCEINLINE constexpr LWidgetSize2(LWidgetSize1 X, f32 Y) noexcept : Type{X.Type}, X{X.Size}, Y{Y} {}
    FORCEINLINE constexpr LWidgetSize2(f32 X, LWidgetSize1 Y) noexcept : Type{Y.Type}, X{X}, Y{Y.Size} {}

    FORCEINLINE constexpr bool operator==(LWidgetSize2 const& Other) const noexcept
    {
        return this->Type == Other.Type && this->Size == Other.Size;
    }
    FORCEINLINE constexpr LWidgetSize2 operator*(f32 Scalar) const noexcept
    {
        return {this->Type, this->Size * Scalar};
    }
};

} /* ~Namespace Jafg */

FORCEINLINE constexpr Jafg::LWidgetSize1 operator ""_spt(unsigned long long Value) noexcept
{
    return {Jafg::EWidgetSize::StaticPoints, static_cast<f32>(Value)};
}

FORCEINLINE constexpr Jafg::LWidgetSize1 operator ""_spt(long double Value) noexcept
{
    return {Jafg::EWidgetSize::StaticPoints, static_cast<f32>(Value)};
}

FORCEINLINE constexpr Jafg::LWidgetSize1 operator ""_pt(unsigned long long Value) noexcept
{
    return {Jafg::EWidgetSize::Points, static_cast<f32>(Value)};
}

FORCEINLINE constexpr Jafg::LWidgetSize1 operator ""_pt(long double Value) noexcept
{
    return {Jafg::EWidgetSize::Points, static_cast<f32>(Value)};
}

FORCEINLINE constexpr Jafg::LWidgetSize2 operator ""_spt2(unsigned long long Value) noexcept
{
    return {Jafg::EWidgetSize::StaticPoints, static_cast<f32>(Value), static_cast<f32>(Value)};
}

FORCEINLINE constexpr Jafg::LWidgetSize2 operator ""_spt2(long double Value) noexcept
{
    return {Jafg::EWidgetSize::StaticPoints, static_cast<f32>(Value), static_cast<f32>(Value)};
}

FORCEINLINE constexpr Jafg::LWidgetSize2 operator ""_pt2(unsigned long long Value) noexcept
{
    return {Jafg::EWidgetSize::Points, static_cast<f32>(Value), static_cast<f32>(Value)};
}

FORCEINLINE constexpr Jafg::LWidgetSize2 operator ""_pt2(long double Value) noexcept
{
    return {Jafg::EWidgetSize::Points, static_cast<f32>(Value), static_cast<f32>(Value)};
}
