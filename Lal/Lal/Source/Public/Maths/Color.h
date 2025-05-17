// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

/**
 * Stores a color of BGRA format with eight bits of precision per channel.
 */
struct LColor final
{
#if PLATFORM_USES_LITTLE_ENDIAN
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
#else /* PLATFORM_USES_LITTLE_ENDIAN */
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
#endif /* !PLATFORM_USES_LITTLE_ENDIAN */

    FORCEINLINE constexpr LColor() noexcept
#if PLATFORM_USES_LITTLE_ENDIAN
        : B(0), G(0), R(0), A(0)
#else /* PLATFORM_USES_LITTLE_ENDIAN */
        : A(0), R(0), G(0), B(0)
#endif /* !PLATFORM_USES_LITTLE_ENDIAN */
    { }

    FORCEINLINE constexpr ~LColor() noexcept = default;

    FORCEINLINE constexpr LColor(const u8 InR, const u8 InG, const u8 InB, const u8 InA = 0xFF) noexcept
#if PLATFORM_USES_LITTLE_ENDIAN
        : B(InB), G(InG), R(InR), A(InA)
#else /* PLATFORM_USES_LITTLE_ENDIAN */
        : A(InA), R(InR), G(InG), B(InB)
#endif /* !PLATFORM_USES_LITTLE_ENDIAN */
    { }
    FORCEINLINE constexpr explicit LColor(const f32 InColor) noexcept
#if PLATFORM_USES_LITTLE_ENDIAN
        : B(static_cast<u8>(InColor * 255.0f)), G(static_cast<u8>(InColor * 255.0f)), R(static_cast<u8>(InColor * 255.0f)), A(0xFF)
#else /* PLATFORM_USES_LITTLE_ENDIAN */
        : A(0xFF), R(static_cast<u8>(InColor * 255.0f)), G(static_cast<u8>(InColor * 255.0f)), B(static_cast<u8>(InColor * 255.0f))
#endif /* !PLATFORM_USES_LITTLE_ENDIAN */
    { }
    FORCEINLINE constexpr explicit LColor(const f64 InColor) noexcept
#if PLATFORM_USES_LITTLE_ENDIAN
        : B(static_cast<u8>(InColor * 255.0)), G(static_cast<u8>(InColor * 255.0)), R(static_cast<u8>(InColor * 255.0)), A(0xFF)
#else /* PLATFORM_USES_LITTLE_ENDIAN */
        : A(0xFF), R(static_cast<u8>(InColor * 255.0)), G(static_cast<u8>(InColor * 255.0)), B(static_cast<u8>(InColor * 255.0))
#endif /* !PLATFORM_USES_LITTLE_ENDIAN */
    { }
    FORCEINLINE constexpr          LColor(const LColor& InColor) noexcept : Bits(InColor.Bits) { }
    FORCEINLINE constexpr explicit LColor(const u32 InColor) noexcept : Bits(InColor) { }
    FORCEINLINE constexpr explicit LColor(u8 InChannels[4]) noexcept
        : Channels{ InChannels[0], InChannels[1], InChannels[2], InChannels[3] } { }

    FORCEINLINE constexpr void operator=(const LColor& Other) noexcept { this->Bits = Other.Bits; }
    FORCEINLINE constexpr void operator=(const u32 InColor)   noexcept { this->Bits = InColor;    }
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

    FORCEINLINE constexpr LVector ToVector3() const noexcept
    {
        return LVector(this->R / 255.0f, this->G / 255.0f, this->B / 255.0f);
    }

    FORCEINLINE constexpr LVector4 ToVector4() const noexcept
    {
        return LVector4(this->R / 255.0f, this->G / 255.0f, this->B / 255.0f, this->A / 255.0f);
    }

    FORCEINLINE constexpr bool operator==(const LColor& Other) const noexcept { return this->Bits == Other.Bits; }
    FORCEINLINE constexpr bool operator!=(const LColor& Other) const noexcept { return this->Bits != Other.Bits; }

    FORCEINLINE constexpr void operator+=(const LColor& Other) noexcept
    {
        this->R += static_cast<u8>(Maths::Min(static_cast<u32>(this->R) + static_cast<u32>(Other.R), 0xFFu));
        this->G += static_cast<u8>(Maths::Min(static_cast<u32>(this->R) + static_cast<u32>(Other.R), 0xFFu));
        this->B += static_cast<u8>(Maths::Min(static_cast<u32>(this->R) + static_cast<u32>(Other.R), 0xFFu));
        this->A += static_cast<u8>(Maths::Min(static_cast<u32>(this->R) + static_cast<u32>(Other.R), 0xFFu));

        return;
    }
    FORCEINLINE constexpr void operator-=(const LColor& Other) noexcept
    {
        this->R -= static_cast<u8>(Maths::Max(static_cast<i32>(this->R) - static_cast<i32>(Other.R), 0x00));
        this->G -= static_cast<u8>(Maths::Max(static_cast<i32>(this->G) - static_cast<i32>(Other.G), 0x00));
        this->B -= static_cast<u8>(Maths::Max(static_cast<i32>(this->B) - static_cast<i32>(Other.B), 0x00));
        this->A -= static_cast<u8>(Maths::Max(static_cast<i32>(this->A) - static_cast<i32>(Other.A), 0x00));

        return;
    }

    LAL_API static const LColor Transparent;
    LAL_API static const LColor White;
    LAL_API static const LColor Black;
    LAL_API static const LColor Gray;
    LAL_API static const LColor Red;
    LAL_API static const LColor DarkRed;
    LAL_API static const LColor Green;
    LAL_API static const LColor Emerald;
    LAL_API static const LColor Blue;
    LAL_API static const LColor Yellow;
    LAL_API static const LColor Cyan;
    LAL_API static const LColor Magenta;

    FORCEINLINE LString ToString() const
    {
        return LString::SprintF("RGBA: {} {} {} {}", this->R, this->G, this->B, this->A);
    }
};

} /* ~Namespace Jafg */
