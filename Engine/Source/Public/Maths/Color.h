// Copyright mzoesch. All rights reserved.

#pragma once

struct LLinearColor;
struct LColor;

//# Stores normalized device ready color data in RGBA format with 32 bits of precision per channel.
struct LLinearColor final
{
    union
    {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        struct { f32 R; f32 G; f32 B; f32 A; };
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        struct { f32 A; f32 B; f32 G; f32 R; };
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    };

    FORCEINLINE constexpr LLinearColor() noexcept = delete;
    FORCEINLINE constexpr LLinearColor(f32 InR, f32 InG, f32 InB, f32 InA = 1.0f) noexcept
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        : R(InR), G(InG), B(InB), A(InA)
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        : A(InA), B(InB), G(InG), R(InR)
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    {
        check(InR >= 0.0f && InR <= 1.0f)
        check(InG >= 0.0f && InG <= 1.0f)
        check(InB >= 0.0f && InB <= 1.0f)
        check(InA >= 0.0f && InA <= 1.0f)
    }
    FORCEINLINE constexpr LLinearColor(f32 InColor) noexcept
        : R(InColor), G(InColor), B(InColor), A(1.0f)
    {
        check(InColor >= 0.0f && InColor <= 1.0f)
    }
    FORCEINLINE constexpr LLinearColor(LLinearColor const& InColor) noexcept
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        : R(InColor.R), G(InColor.G), B(InColor.B), A(InColor.A)
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        : A(InColor.A), B(InColor.B), G(InColor.G), R(InColor.R)
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    {
    }
    FORCEINLINE constexpr LLinearColor& operator=(LLinearColor const& InColor) noexcept
    {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        this->R = InColor.R;
        this->G = InColor.G;
        this->B = InColor.B;
        this->A = InColor.A;
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        this->A = InColor.A;
        this->B = InColor.B;
        this->G = InColor.G;
        this->R = InColor.R;
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        return *this;
    }
    FORCEINLINE constexpr LLinearColor(LLinearColor&& InColor) noexcept
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        : R(InColor.R), G(InColor.G), B(InColor.B), A(InColor.A)
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        : A(InColor.A), B(InColor.B), G(InColor.G), R(InColor.R)
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    {
    }
    FORCEINLINE constexpr LLinearColor& operator=(LLinearColor&& InColor) noexcept
    {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        this->R = InColor.R;
        this->G = InColor.G;
        this->B = InColor.B;
        this->A = InColor.A;
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        this->A = InColor.A;
        this->B = InColor.B;
        this->G = InColor.G;
        this->R = InColor.R;
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        return *this;
    }
    FORCEINLINE constexpr ~LLinearColor() noexcept = default;

    NODISCARD FORCEINLINE constexpr LVec3F ToDeviceReadyVector3() const noexcept
    {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        return {this->R, this->G, this->B};
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        return {this->B, this->G, this->R};
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    }
    NODISCARD FORCEINLINE constexpr LVec4F ToDeviceReadyVector4() const noexcept
    {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        return {this->R, this->G, this->B, this->A};
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        return {this->A, this->B, this->G, this->R};
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    }
    NODISCARD FORCEINLINE constexpr LColor ToColor() const noexcept;
    NODISCARD FORCEINLINE static constexpr LLinearColor FromColor(LColor const& InColor) noexcept;

    FORCEINLINE bool operator==(LLinearColor const& Other) const noexcept { return std::memcmp(this, &Other, sizeof(LLinearColor)) == 0; }
    FORCEINLINE constexpr void operator+=(LLinearColor const& Other) noexcept
    {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        this->R += maths::min(this->R + Other.R, 1.0f);
        this->G += maths::min(this->G + Other.G, 1.0f);
        this->B += maths::min(this->B + Other.B, 1.0f);
        this->A += maths::min(this->A + Other.A, 1.0f);
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        this->A += maths::min(this->A + Other.A, 1.0f);
        this->B += maths::min(this->B + Other.B, 1.0f);
        this->G += maths::min(this->G + Other.G, 1.0f);
        this->R += maths::min(this->R + Other.R, 1.0f);
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    }
    FORCEINLINE constexpr void operator-=(LLinearColor const& Other) noexcept
    {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        this->R -= maths::max(this->R - Other.R, 0.0f);
        this->G -= maths::max(this->G - Other.G, 0.0f);
        this->B -= maths::max(this->B - Other.B, 0.0f);
        this->A -= maths::max(this->A - Other.A, 0.0f);
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        this->A -= maths::max(this->A - Other.A, 0.0f);
        this->B -= maths::max(this->B - Other.B, 0.0f);
        this->G -= maths::max(this->G - Other.G, 0.0f);
        this->R -= maths::max(this->R - Other.R, 0.0f);
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    }
};

//# Stores device ready color data in RGBA format with eight bits of precision per channel.
struct LColor final
{
    union
    {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        struct { u8 R; u8 G; u8 B; u8 A; };
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        struct { u8 A; u8 B; u8 G; u8 R; };
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        u32 Bits;
    };

    FORCEINLINE constexpr LColor() noexcept = delete;
    FORCEINLINE constexpr LColor(u8 InR, u8 InG, u8 InB, u8 InA = 0xFF) noexcept
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        : R(InR), G(InG), B(InB), A(InA)
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        : A(InA), B(InB), G(InG), R(InR)
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    {
    }
    FORCEINLINE constexpr LColor(u8 InColor) noexcept
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        : R(InColor), G(InColor), B(InColor), A(0xFF)
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        : A(0xFF), B(InColor), G(InColor), R(InColor)
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    {
    }
    FORCEINLINE constexpr LColor(LColor const& InColor) noexcept : Bits(InColor.Bits) {}
    FORCEINLINE constexpr LColor& operator=(LColor const& Rhs) noexcept { this->Bits = Rhs.Bits; return *this; }
    FORCEINLINE constexpr LColor(LColor&& InColor) noexcept : Bits(InColor.Bits) {}
    FORCEINLINE constexpr LColor& operator=(LColor&& Rhs) noexcept { this->Bits = Rhs.Bits; return *this; }
    FORCEINLINE constexpr ~LColor() noexcept = default;

    NODISCARD FORCEINLINE constexpr LVec3F ToDeviceReadyVector3() const noexcept
    {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        return {this->R / 255.0f, this->G / 255.0f, this->B / 255.0f};
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        return {this->B / 255.0f, this->G / 255.0f, this->R / 255.0f};
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    }
    NODISCARD FORCEINLINE constexpr LVec4F ToDeviceReadyVector4() const noexcept
    {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        return {this->R / 255.0f, this->G / 255.0f, this->B / 255.0f, this->A / 255.0f};
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        return {this->A / 255.0f, this->B / 255.0f, this->G / 255.0f, this->R / 255.0f};
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    }

    NODISCARD FORCEINLINE constexpr LLinearColor ToLinearColor() const noexcept
    {
        return {this->R / 255.0f, this->G / 255.0f, this->B / 255.0f, this->A / 255.0f,};
    }
    NODISCARD FORCEINLINE static constexpr LColor FromLinearColor(LLinearColor const& InColor) noexcept
    {
        return {
            static_cast<u8>(InColor.R * 255.0f),
            static_cast<u8>(InColor.G * 255.0f),
            static_cast<u8>(InColor.B * 255.0f),
            static_cast<u8>(InColor.A * 255.0f),
            };
    }

    FORCEINLINE constexpr bool operator==(LColor const& Other) const noexcept { return this->Bits == Other.Bits; }
    FORCEINLINE constexpr void operator+=(LColor const& Other) noexcept
    {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        this->R += static_cast<u8>(maths::min(static_cast<u32>(this->R) + static_cast<u32>(Other.R), 0xFFu));
        this->G += static_cast<u8>(maths::min(static_cast<u32>(this->G) + static_cast<u32>(Other.G), 0xFFu));
        this->B += static_cast<u8>(maths::min(static_cast<u32>(this->B) + static_cast<u32>(Other.B), 0xFFu));
        this->A += static_cast<u8>(maths::min(static_cast<u32>(this->A) + static_cast<u32>(Other.A), 0xFFu));
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        this->A += static_cast<u8>(maths::min(static_cast<u32>(this->A) + static_cast<u32>(Other.A), 0xFFu));
        this->B += static_cast<u8>(maths::min(static_cast<u32>(this->B) + static_cast<u32>(Other.B), 0xFFu));
        this->G += static_cast<u8>(maths::min(static_cast<u32>(this->G) + static_cast<u32>(Other.G), 0xFFu));
        this->R += static_cast<u8>(maths::min(static_cast<u32>(this->R) + static_cast<u32>(Other.R), 0xFFu));
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    }
    FORCEINLINE constexpr void operator-=(LColor const& Other) noexcept
    {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        this->R -= static_cast<u8>(maths::max(static_cast<i32>(this->R) - static_cast<i32>(Other.R), 0x00));
        this->G -= static_cast<u8>(maths::max(static_cast<i32>(this->G) - static_cast<i32>(Other.G), 0x00));
        this->B -= static_cast<u8>(maths::max(static_cast<i32>(this->B) - static_cast<i32>(Other.B), 0x00));
        this->A -= static_cast<u8>(maths::max(static_cast<i32>(this->A) - static_cast<i32>(Other.A), 0x00));
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        this->A -= static_cast<u8>(maths::max(static_cast<i32>(this->A) - static_cast<i32>(Other.A), 0x00));
        this->B -= static_cast<u8>(maths::max(static_cast<i32>(this->B) - static_cast<i32>(Other.B), 0x00));
        this->G -= static_cast<u8>(maths::max(static_cast<i32>(this->G) - static_cast<i32>(Other.G), 0x00));
        this->R -= static_cast<u8>(maths::max(static_cast<i32>(this->R) - static_cast<i32>(Other.R), 0x00));
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    }
};

FORCEINLINE constexpr LColor LLinearColor::ToColor() const noexcept
{
    return {
        static_cast<u8>(this->R * 255.0f),
        static_cast<u8>(this->G * 255.0f),
        static_cast<u8>(this->B * 255.0f),
        static_cast<u8>(this->A * 255.0f),
        };
}

FORCEINLINE constexpr LLinearColor LLinearColor::FromColor(LColor const& InColor) noexcept
{
    return {
        InColor.R / 255.0f,
        InColor.G / 255.0f,
        InColor.B / 255.0f,
        InColor.A / 255.0f,
        };
}

namespace Colors
{

#define PRIVATE_JAFG_COLOR_VALUE LColor
#define PRIVATE_JAFG_COLOR_VALUE_CONV(x,y,z,w) x,y,z,w
#include "Maths/ColorValues.h"

} /* ~Namespace Colors */

namespace LinearColors
{

#define PRIVATE_JAFG_COLOR_VALUE LLinearColor
#define PRIVATE_JAFG_COLOR_VALUE_CONV(x,y,z,w) LLinearColor::FromColor(LColor{x,y,z,w})
#include "Maths/ColorValues.h"

} /* ~Namespace LinearColors */
