// Copyright mzoesch. All rights reserved.

#pragma once

struct LLinearColor;
struct LColor;

//# Stores a color of BGRA format with 32 bits of precision per channel.
struct LLinearColor final
{
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
    union
    {
        struct
        {
            f32 B;
            f32 G;
            f32 R;
            f32 A;
        };

        f32 Channels[4];
    };
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    union
    {
        struct
        {
            f32 A;
            f32 R;
            f32 G;
            f32 B;
        };

        f32  Channels[4];
    };
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */

    FORCEINLINE constexpr LLinearColor() noexcept
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        : B(0.0f), G(0.0f), R(0.0f), A(1.0f)
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        : A(0.0f), R(0.0f), G(0.0f), B(0.0f)
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    {
    }

    FORCEINLINE constexpr LLinearColor(const f32 InR, const f32 InG, const f32 InB, const f32 InA = 1.0f) noexcept
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        : B(InB), G(InG), R(InR), A(InA)
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        : A(InA), R(InR), G(InG), B(InB)
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    {
        check( InR >= 0.0f && InR <= 1.0f )
        check( InG >= 0.0f && InG <= 1.0f )
        check( InB >= 0.0f && InB <= 1.0f )
        check( InA >= 0.0f && InA <= 1.0f )

        return;
    }

    FORCEINLINE constexpr LLinearColor(const f32 InColor) noexcept
        : B(InColor), G(InColor), R(InColor), A(1.0f)
    {
        check( InColor >= 0.0f && InColor <= 1.0f )
        return;
    }

    FORCEINLINE constexpr LLinearColor(const f64 InColor) noexcept
        : B(static_cast<f32>(InColor)), G(static_cast<f32>(InColor)), R(static_cast<f32>(InColor)), A(1.0f)
    {
        check( InColor >= 0.0f && InColor <= 1.0f )
        return;
    }

    FORCEINLINE constexpr ~LLinearColor() noexcept = default;

    FORCEINLINE constexpr void operator=(const LLinearColor& InColor) noexcept
    {
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        this->B = InColor.B;
        this->G = InColor.G;
        this->R = InColor.R;
        this->A = InColor.A;
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        this->A = InColor.A;
        this->R = InColor.R;
        this->G = InColor.G;
        this->B = InColor.B;
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    }

    NODISCARD FORCEINLINE constexpr LVec3F ToVector3() const noexcept { return {this->R, this->G, this->B}; }
    NODISCARD FORCEINLINE constexpr LVec4F ToVector4() const noexcept { return {this->R, this->G, this->B, this->A}; }
    NODISCARD FORCEINLINE constexpr LColor ToColor() const noexcept;

    FORCEINLINE static constexpr LLinearColor FromColor(LColor const& InColor) noexcept;
    FORCEINLINE static constexpr LLinearColor FromColor(const u8 InR, const u8 InG, const u8 InB, const u8 InA) noexcept
    {
        return {
            static_cast<f32>(InR) / 255.0f,
            static_cast<f32>(InG) / 255.0f,
            static_cast<f32>(InB) / 255.0f,
            static_cast<f32>(InA) / 255.0f,
            };
    }

    FORCEINLINE bool operator==(LLinearColor const& Other) const noexcept { return std::memcmp(this, &Other, sizeof(LLinearColor)) == 0; }
    FORCEINLINE bool operator!=(LLinearColor const& Other) const noexcept { return std::memcmp(this, &Other, sizeof(LLinearColor)) != 0; }

    FORCEINLINE constexpr void operator+=(LLinearColor const& Other) noexcept
    {
        this->R += maths::min(this->R + Other.R, 1.0f);
        this->G += maths::min(this->R + Other.R, 1.0f);
        this->B += maths::min(this->R + Other.R, 1.0f);
        this->A += maths::min(this->R + Other.R, 1.0f);

        return;
    }
    FORCEINLINE constexpr void operator-=(LLinearColor const& Other) noexcept
    {
        this->R -= maths::max(this->R - Other.R, 0.0f);
        this->G -= maths::max(this->G - Other.G, 0.0f);
        this->B -= maths::max(this->B - Other.B, 0.0f);
        this->A -= maths::max(this->A - Other.A, 0.0f);

        return;
    }

    FORCEINLINE LString ToString() const noexcept
    {
        return Jafg::SprintF("RGBA: {} {} {} {}", this->R, this->G, this->B, this->A);
    }


};

/**
 * Stores a color of BGRA format with eight bits of precision per channel.
 */
struct LColor final
{
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
    union
    {
        struct
        {
            u8 B;
            u8 G;
            u8 R;
            u8 A;
        };

        u32 Bits = 0x00u;

        u8 Channels[4];
    };
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    union
    {
        struct
        {
            u8 A;
            u8 R;
            u8 G;
            u8 B;
        };

        u32 Bits = 0x00u;

        u8  Channels[4];
    };
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */

    FORCEINLINE constexpr LColor() noexcept = delete;

    FORCEINLINE constexpr LColor(const u8 InR, const u8 InG, const u8 InB, const u8 InA = 0xFF) noexcept
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        : B(InB), G(InG), R(InR), A(InA)
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        : A(InA), R(InR), G(InG), B(InB)
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    {
    }

    FORCEINLINE constexpr LColor(const u8 InColor) noexcept
#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
        : B(InColor), G(InColor), R(InColor), A(0xFF)
#else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        : A(InA), R(InR), G(InG), B(InB)
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
    {
    }

    FORCEINLINE constexpr LColor(LColor const& InColor) noexcept
        : Bits(InColor.Bits)
    {
    }

    FORCEINLINE constexpr explicit LColor(u8 InChannels[4]) noexcept
        : Channels{ InChannels[0], InChannels[1], InChannels[2], InChannels[3] }
    {
    }

    FORCEINLINE constexpr ~LColor() noexcept = default;

    FORCEINLINE constexpr void operator=(LColor const& Rhs) noexcept { this->Bits = Rhs.Bits; }
    FORCEINLINE constexpr void operator=(const u8 InChannels[]) noexcept
    {
        this->Channels[0] = InChannels[0];
        this->Channels[1] = InChannels[1];
        this->Channels[2] = InChannels[2];
        this->Channels[3] = InChannels[3];

        return;
    }

    FORCEINLINE constexpr u32 RGBA() const noexcept
    {
        return (static_cast<u32>(R) << 24) | (static_cast<u32>(G) << 16) | (static_cast<u32>(B) << 8) | static_cast<u32>(A);
    }
    FORCEINLINE constexpr u32 BGRA() const noexcept
    {
        return (static_cast<u32>(B) << 24) | (static_cast<u32>(G) << 16) | (static_cast<u32>(R) << 8) | static_cast<u32>(A);
    }

    FORCEINLINE constexpr LVec3F ToVector3() const noexcept
    {
        return {this->R / 255.0f, this->G / 255.0f, this->B / 255.0f};
    }

    FORCEINLINE constexpr LVec4F ToVector4() const noexcept
    {
        return {this->R / 255.0f, this->G / 255.0f, this->B / 255.0f, this->A / 255.0f};
    }

    FORCEINLINE constexpr LLinearColor ToLinearColor() const noexcept
    {
        return {
            this->R / 255.0f,
            this->G / 255.0f,
            this->B / 255.0f,
            this->A / 255.0f,
            };
    }

    FORCEINLINE static constexpr LColor FromLinearColor(const LLinearColor& InColor) noexcept
    {
        return {
            static_cast<u8>(InColor.R * 255.0f),
            static_cast<u8>(InColor.G * 255.0f),
            static_cast<u8>(InColor.B * 255.0f),
            static_cast<u8>(InColor.A * 255.0f),
            };
    }

    FORCEINLINE constexpr bool operator==(LColor const& Other) const noexcept { return this->Bits == Other.Bits; }
    FORCEINLINE constexpr bool operator!=(LColor const& Other) const noexcept { return this->Bits != Other.Bits; }

    FORCEINLINE constexpr void operator+=(LColor const& Other) noexcept
    {
        this->R += static_cast<u8>(maths::min(static_cast<u32>(this->R) + static_cast<u32>(Other.R), 0xFFu));
        this->G += static_cast<u8>(maths::min(static_cast<u32>(this->R) + static_cast<u32>(Other.R), 0xFFu));
        this->B += static_cast<u8>(maths::min(static_cast<u32>(this->R) + static_cast<u32>(Other.R), 0xFFu));
        this->A += static_cast<u8>(maths::min(static_cast<u32>(this->R) + static_cast<u32>(Other.R), 0xFFu));

        return;
    }
    FORCEINLINE constexpr void operator-=(LColor const& Other) noexcept
    {
        this->R -= static_cast<u8>(maths::max(static_cast<i32>(this->R) - static_cast<i32>(Other.R), 0x00));
        this->G -= static_cast<u8>(maths::max(static_cast<i32>(this->G) - static_cast<i32>(Other.G), 0x00));
        this->B -= static_cast<u8>(maths::max(static_cast<i32>(this->B) - static_cast<i32>(Other.B), 0x00));
        this->A -= static_cast<u8>(maths::max(static_cast<i32>(this->A) - static_cast<i32>(Other.A), 0x00));

        return;
    }

    FORCEINLINE LString ToString() const
    {
        return Jafg::SprintF("RGBA: {} {} {} {}", this->R, this->G, this->B, this->A);
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
#define PRIVATE_JAFG_COLOR_VALUE_CONV(x,y,z,w) LLinearColor::FromColor(x,y,z,w)
#include "Maths/ColorValues.h"

} /* ~Namespace LinearColors */
