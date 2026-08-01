// Copyright mzoesch. All rights reserved.

#pragma once

struct LLinearColor;
struct LColor;

//# Stores normalized device ready color data in linear RGBA format with 32 bits of precision per channel.
struct LLinearColor final
{
    typedef f32 value_type;

    value_type r;
    value_type g;
    value_type b;
    value_type a;

    FORCEINLINE constexpr LLinearColor() noexcept = delete;
    FORCEINLINE constexpr LLinearColor(ESkipInit) noexcept {}
    FORCEINLINE constexpr LLinearColor(value_type R, value_type G, value_type B, value_type A = 1.0f) noexcept
        : r(R), g(G), b(B), a(A) {}
    FORCEINLINE constexpr LLinearColor(value_type Uniform) noexcept
        : r(Uniform), g(Uniform), b(Uniform), a(1.0f) {}
    FORCEINLINE constexpr LLinearColor(LLinearColor const&) noexcept = default;
    FORCEINLINE constexpr LLinearColor& operator=(LLinearColor const&) noexcept = default;
    FORCEINLINE constexpr ~LLinearColor() noexcept = default;

    NODISCARD FORCEINLINE constexpr LVec3F ToDeviceReadyVector3() const noexcept { return {this->r, this->g, this->b}; }
    NODISCARD FORCEINLINE constexpr LVec4F ToDeviceReadyVector4() const noexcept { return {this->r, this->g, this->b, this->a}; }

    NODISCARD FORCEINLINE static constexpr LLinearColor FromStandardRGB(LColor const& Color) noexcept;

    FORCEINLINE bool operator==(LLinearColor const& Rhs) const noexcept { return std::memcmp(this, &Rhs, sizeof(LLinearColor)) == 0; }

    FORCEINLINE constexpr LLinearColor operator+(LLinearColor const& Other) const noexcept
    {
        return {this->r + Other.r, this->g + Other.g, this->b + Other.b, this->a + Other.a};
    }
    FORCEINLINE constexpr LLinearColor operator+(value_type Scalar) const noexcept
    {
        return {this->r + Scalar, this->g + Scalar, this->b + Scalar, this->a + Scalar};
    }

    FORCEINLINE constexpr LLinearColor& operator+=(LLinearColor const& Other) noexcept
    {
        this->r += Other.r;
        this->g += Other.g;
        this->b += Other.b;
        this->a += Other.a;
        return *this;
    }
    FORCEINLINE constexpr LLinearColor& operator+=(value_type Scalar) noexcept
    {
        this->r += Scalar;
        this->g += Scalar;
        this->b += Scalar;
        this->a += Scalar;
        return *this;
    }

    FORCEINLINE constexpr LLinearColor operator-(LLinearColor const& Other) const noexcept
    {
        return {this->r - Other.r, this->g - Other.g, this->b - Other.b, this->a - Other.a};
    }
    FORCEINLINE constexpr LLinearColor operator-(value_type Scalar) const noexcept
    {
        return {this->r - Scalar, this->g - Scalar, this->b - Scalar, this->a - Scalar};
    }

    FORCEINLINE constexpr LLinearColor& operator-=(LLinearColor const& Other) noexcept
    {
        this->r -=Other.r;
        this->g -=Other.g;
        this->b -=Other.b;
        this->a -=Other.a;
        return *this;
    }
    FORCEINLINE constexpr LLinearColor& operator-=(value_type Scalar) noexcept
    {
        this->r -= Scalar;
        this->g -= Scalar;
        this->b -= Scalar;
        this->a -= Scalar;
        return *this;
    }

    FORCEINLINE constexpr LLinearColor operator*(LLinearColor const& Other) const noexcept
    {
        return {this->r * Other.r, this->g * Other.g, this->b * Other.b, this->a * Other.a};
    }
    FORCEINLINE constexpr LLinearColor operator*(value_type Scalar) const noexcept
    {
        return {this->r * Scalar, this->g * Scalar, this->b * Scalar, this->a * Scalar};
    }

    FORCEINLINE constexpr LLinearColor& operator*=(LLinearColor const& Other) noexcept
    {
        this->r *= Other.r;
        this->g *= Other.g;
        this->b *= Other.b;
        this->a *= Other.a;
        return *this;
    }
    FORCEINLINE constexpr LLinearColor& operator*=(value_type Scalar) noexcept
    {
        this->r *= Scalar;
        this->g *= Scalar;
        this->b *= Scalar;
        this->a *= Scalar;
        return *this;
    }

    FORCEINLINE constexpr LLinearColor operator/(LLinearColor const& Other) const noexcept
    {
        return {this->r / Other.r, this->g / Other.g, this->b / Other.b, this->a / Other.a};
    }
    FORCEINLINE constexpr LLinearColor operator/(value_type Scalar) const noexcept
    {
        return {this->r / Scalar, this->g / Scalar, this->b / Scalar, this->a / Scalar};
    }

    FORCEINLINE constexpr LLinearColor& operator/=(LLinearColor const& Other) noexcept
    {
        this->r /= Other.r;
        this->g /= Other.g;
        this->b /= Other.b;
        this->a /= Other.a;
        return *this;
    }
    FORCEINLINE constexpr LLinearColor& operator/=(value_type Scalar) noexcept
    {
        this->r /= Scalar;
        this->g /= Scalar;
        this->b /= Scalar;
        this->a /= Scalar;
        return *this;
    }
};

//# Stores device ready color data in standard RGBA format with eight bits of precision per channel.
struct LColor final
{
    typedef u8 value_type;

    union
    {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        struct{ value_type r; value_type g; value_type b; value_type a; };
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        struct{ value_type a; value_type b; value_type g; value_type r; };
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        u32 Bits;
    };

    FORCEINLINE constexpr LColor() noexcept = delete;
    FORCEINLINE constexpr LColor(ESkipInit) noexcept {}
    FORCEINLINE constexpr LColor(value_type R, value_type G, value_type B, value_type A = 0xFF) noexcept
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        : r(R), g(G), b(B), a(A)
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        : a(A), b(B), g(G), r(R)
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    {
    }
    FORCEINLINE constexpr LColor(value_type InColor) noexcept
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        : r(InColor), g(InColor), b(InColor), a(0xFF)
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        : a(0xFF), b(InColor), g(InColor), r(InColor)
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    {
    }
    FORCEINLINE constexpr LColor(LColor const& InColor) noexcept : Bits(InColor.Bits) {}
    FORCEINLINE constexpr LColor& operator=(LColor const& Rhs) noexcept { this->Bits = Rhs.Bits; return *this; }
    FORCEINLINE constexpr ~LColor() noexcept = default;

    FORCEINLINE constexpr bool operator==(LColor const& Other) const noexcept { return this->Bits == Other.Bits; }

    FORCEINLINE constexpr LColor operator+(LColor const& Other) const noexcept
    {
        return {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
            static_cast<value_type>(maths::min(static_cast<u32>(this->r) + static_cast<u32>(Other.r), 0xFFu)),
            static_cast<value_type>(maths::min(static_cast<u32>(this->g) + static_cast<u32>(Other.g), 0xFFu)),
            static_cast<value_type>(maths::min(static_cast<u32>(this->b) + static_cast<u32>(Other.b), 0xFFu)),
            static_cast<value_type>(maths::min(static_cast<u32>(this->a) + static_cast<u32>(Other.a), 0xFFu)),
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
            static_cast<value_type>(maths::min(static_cast<u32>(this->a) + static_cast<u32>(Other.a), 0xFFu)),
            static_cast<value_type>(maths::min(static_cast<u32>(this->b) + static_cast<u32>(Other.b), 0xFFu)),
            static_cast<value_type>(maths::min(static_cast<u32>(this->g) + static_cast<u32>(Other.g), 0xFFu)),
            static_cast<value_type>(maths::min(static_cast<u32>(this->r) + static_cast<u32>(Other.r), 0xFFu)),
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
            };
    }
    FORCEINLINE constexpr LColor operator+(value_type Scalar) const noexcept
    {
        return {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
            static_cast<value_type>(maths::min(static_cast<u32>(this->r) + static_cast<u32>(Scalar), 0xFFu)),
            static_cast<value_type>(maths::min(static_cast<u32>(this->g) + static_cast<u32>(Scalar), 0xFFu)),
            static_cast<value_type>(maths::min(static_cast<u32>(this->b) + static_cast<u32>(Scalar), 0xFFu)),
            static_cast<value_type>(maths::min(static_cast<u32>(this->a) + static_cast<u32>(Scalar), 0xFFu)),
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
            static_cast<value_type>(maths::min(static_cast<u32>(this->a) + static_cast<u32>(Scalar), 0xFFu)),
            static_cast<value_type>(maths::min(static_cast<u32>(this->b) + static_cast<u32>(Scalar), 0xFFu)),
            static_cast<value_type>(maths::min(static_cast<u32>(this->g) + static_cast<u32>(Scalar), 0xFFu)),
            static_cast<value_type>(maths::min(static_cast<u32>(this->r) + static_cast<u32>(Scalar), 0xFFu)),
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
            };
    }

    FORCEINLINE constexpr LColor& operator+=(LColor const& Other) noexcept
    {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        this->r = static_cast<value_type>(maths::min(static_cast<u32>(this->r) + static_cast<u32>(Other.r), 0xFFu));
        this->g = static_cast<value_type>(maths::min(static_cast<u32>(this->g) + static_cast<u32>(Other.g), 0xFFu));
        this->b = static_cast<value_type>(maths::min(static_cast<u32>(this->b) + static_cast<u32>(Other.b), 0xFFu));
        this->a = static_cast<value_type>(maths::min(static_cast<u32>(this->a) + static_cast<u32>(Other.a), 0xFFu));
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        this->a = static_cast<value_type>(maths::min(static_cast<u32>(this->a) + static_cast<u32>(Other.a), 0xFFu));
        this->b = static_cast<value_type>(maths::min(static_cast<u32>(this->b) + static_cast<u32>(Other.b), 0xFFu));
        this->g = static_cast<value_type>(maths::min(static_cast<u32>(this->g) + static_cast<u32>(Other.g), 0xFFu));
        this->r = static_cast<value_type>(maths::min(static_cast<u32>(this->r) + static_cast<u32>(Other.r), 0xFFu));
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        return *this;
    }
    FORCEINLINE constexpr LColor& operator+=(value_type Scalar) noexcept
    {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        this->r = static_cast<value_type>(maths::min(static_cast<u32>(this->r) + static_cast<u32>(Scalar), 0xFFu));
        this->g = static_cast<value_type>(maths::min(static_cast<u32>(this->g) + static_cast<u32>(Scalar), 0xFFu));
        this->b = static_cast<value_type>(maths::min(static_cast<u32>(this->b) + static_cast<u32>(Scalar), 0xFFu));
        this->a = static_cast<value_type>(maths::min(static_cast<u32>(this->a) + static_cast<u32>(Scalar), 0xFFu));
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        this->a = static_cast<value_type>(maths::min(static_cast<u32>(this->a) + static_cast<u32>(Scalar), 0xFFu));
        this->b = static_cast<value_type>(maths::min(static_cast<u32>(this->b) + static_cast<u32>(Scalar), 0xFFu));
        this->g = static_cast<value_type>(maths::min(static_cast<u32>(this->g) + static_cast<u32>(Scalar), 0xFFu));
        this->r = static_cast<value_type>(maths::min(static_cast<u32>(this->r) + static_cast<u32>(Scalar), 0xFFu));
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        return *this;
    }

    FORCEINLINE constexpr LColor operator-(LColor const& Other) const noexcept
    {
        return {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
            static_cast<value_type>(maths::max(static_cast<i32>(this->r) - static_cast<i32>(Other.r), 0x00)),
            static_cast<value_type>(maths::max(static_cast<i32>(this->g) - static_cast<i32>(Other.g), 0x00)),
            static_cast<value_type>(maths::max(static_cast<i32>(this->b) - static_cast<i32>(Other.b), 0x00)),
            static_cast<value_type>(maths::max(static_cast<i32>(this->a) - static_cast<i32>(Other.a), 0x00)),
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
            static_cast<value_type>(maths::max(static_cast<i32>(this->a) - static_cast<i32>(Other.a), 0x00)),
            static_cast<value_type>(maths::max(static_cast<i32>(this->b) - static_cast<i32>(Other.b), 0x00)),
            static_cast<value_type>(maths::max(static_cast<i32>(this->g) - static_cast<i32>(Other.g), 0x00)),
            static_cast<value_type>(maths::max(static_cast<i32>(this->r) - static_cast<i32>(Other.r), 0x00)),
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
            };
    }
    FORCEINLINE constexpr LColor operator-(value_type Scalar) const noexcept
    {
        return {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
            static_cast<value_type>(maths::max(static_cast<i32>(this->r) - static_cast<i32>(Scalar), 0x00)),
            static_cast<value_type>(maths::max(static_cast<i32>(this->g) - static_cast<i32>(Scalar), 0x00)),
            static_cast<value_type>(maths::max(static_cast<i32>(this->b) - static_cast<i32>(Scalar), 0x00)),
            static_cast<value_type>(maths::max(static_cast<i32>(this->a) - static_cast<i32>(Scalar), 0x00)),
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
            static_cast<value_type>(maths::max(static_cast<i32>(this->a) - static_cast<i32>(Scalar), 0x00)),
            static_cast<value_type>(maths::max(static_cast<i32>(this->b) - static_cast<i32>(Scalar), 0x00)),
            static_cast<value_type>(maths::max(static_cast<i32>(this->g) - static_cast<i32>(Scalar), 0x00)),
            static_cast<value_type>(maths::max(static_cast<i32>(this->r) - static_cast<i32>(Scalar), 0x00)),
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
            };
    }

    FORCEINLINE constexpr LColor& operator-=(LColor const& Other) noexcept
    {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        this->r = static_cast<value_type>(maths::max(static_cast<i32>(this->r) - static_cast<i32>(Other.r), 0x00));
        this->g = static_cast<value_type>(maths::max(static_cast<i32>(this->g) - static_cast<i32>(Other.g), 0x00));
        this->b = static_cast<value_type>(maths::max(static_cast<i32>(this->b) - static_cast<i32>(Other.b), 0x00));
        this->a = static_cast<value_type>(maths::max(static_cast<i32>(this->a) - static_cast<i32>(Other.a), 0x00));
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        this->a = static_cast<value_type>(maths::max(static_cast<i32>(this->a) - static_cast<i32>(Other.a), 0x00));
        this->b = static_cast<value_type>(maths::max(static_cast<i32>(this->b) - static_cast<i32>(Other.b), 0x00));
        this->g = static_cast<value_type>(maths::max(static_cast<i32>(this->g) - static_cast<i32>(Other.g), 0x00));
        this->r = static_cast<value_type>(maths::max(static_cast<i32>(this->r) - static_cast<i32>(Other.r), 0x00));
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        return *this;
    }
    FORCEINLINE constexpr LColor& operator-=(value_type Scalar) noexcept
    {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        this->r = static_cast<value_type>(maths::max(static_cast<i32>(this->r) - static_cast<i32>(Scalar), 0x00));
        this->g = static_cast<value_type>(maths::max(static_cast<i32>(this->g) - static_cast<i32>(Scalar), 0x00));
        this->b = static_cast<value_type>(maths::max(static_cast<i32>(this->b) - static_cast<i32>(Scalar), 0x00));
        this->a = static_cast<value_type>(maths::max(static_cast<i32>(this->a) - static_cast<i32>(Scalar), 0x00));
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        this->a = static_cast<value_type>(maths::max(static_cast<i32>(this->a) - static_cast<i32>(Scalar), 0x00));
        this->b = static_cast<value_type>(maths::max(static_cast<i32>(this->b) - static_cast<i32>(Scalar), 0x00));
        this->g = static_cast<value_type>(maths::max(static_cast<i32>(this->g) - static_cast<i32>(Scalar), 0x00));
        this->r = static_cast<value_type>(maths::max(static_cast<i32>(this->r) - static_cast<i32>(Scalar), 0x00));
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        return *this;
    }
};

FORCEINLINE constexpr LLinearColor LLinearColor::FromStandardRGB(LColor const& Color) noexcept
{
    return {
        maths::srgb_to_linear_table[Color.r],
        maths::srgb_to_linear_table[Color.g],
        maths::srgb_to_linear_table[Color.b],
        static_cast<value_type>(Color.a) * (1.0f/255.0f),
        };
}

FORCEINLINE constexpr LLinearColor operator*(LLinearColor::value_type Scalar, LLinearColor const& Color) noexcept { return Color * Scalar; }
FORCEINLINE constexpr LLinearColor operator+(LLinearColor::value_type Scalar, LLinearColor const& Color) noexcept { return Color + Scalar; }
FORCEINLINE constexpr LColor operator+(LColor::value_type Scalar, LColor const& Color) noexcept { return Color + Scalar; }

static_assert(maths::is_color_v<LColor>);
static_assert(maths::is_color_v<LLinearColor>);

namespace Colors
{

#define PRIVATE_JAFG_COLOR_VALUE LColor
#define PRIVATE_JAFG_COLOR_VALUE_CONV(x,y,z,w) x,y,z,w
#include "Core/ColorValues.h"

} /* ~Namespace Colors */

namespace LinearColors
{

#define PRIVATE_JAFG_COLOR_VALUE LLinearColor
#define PRIVATE_JAFG_COLOR_VALUE_CONV(x,y,z,w) LLinearColor::FromStandardRGB(LColor{x,y,z,w})
#include "Core/ColorValues.h"

} /* ~Namespace LinearColors */

FORCEINLINE constexpr LColor operator ""_color(unsigned long long Value) noexcept
{
    return LColor{
        static_cast<uint8_t>((Value >> 24) & 0xFF),
        static_cast<uint8_t>((Value >> 16) & 0xFF),
        static_cast<uint8_t>((Value >> 8) & 0xFF),
        static_cast<uint8_t>(Value & 0xFF)
        };
}
