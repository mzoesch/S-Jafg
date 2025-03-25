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

        u8  Channels[4];
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

    FORCEINLINE  LColor() { }
    FORCEINLINE ~LColor() { }

    FORCEINLINE constexpr LColor(const u8 InR, const u8 InG, const u8 InB, const u8 InA = 0xFF)
#if PLATFORM_USES_LITTLE_ENDIAN
        : B(InB), G(InG), R(InR), A(InA)
#else /* PLATFORM_USES_LITTLE_ENDIAN */
        : A(InA), R(InR), G(InG), B(InB)
#endif /* !PLATFORM_USES_LITTLE_ENDIAN */
    { }
    FORCEINLINE constexpr explicit LColor(const float InColor)
#if PLATFORM_USES_LITTLE_ENDIAN
        : B(static_cast<u8>(InColor * 255.0f)), G(static_cast<u8>(InColor * 255.0f)), R(static_cast<u8>(InColor * 255.0f)), A(0xFF)
#else /* PLATFORM_USES_LITTLE_ENDIAN */
        : A(0xFF), R(static_cast<u8>(InColor * 255.0f)), G(static_cast<u8>(InColor * 255.0f)), B(static_cast<u8>(InColor * 255.0f))
#endif /* !PLATFORM_USES_LITTLE_ENDIAN */
    { }
    FORCEINLINE constexpr explicit LColor(const double InColor)
#if PLATFORM_USES_LITTLE_ENDIAN
        : B(static_cast<u8>(InColor * 255.0)), G(static_cast<u8>(InColor * 255.0)), R(static_cast<u8>(InColor * 255.0)), A(0xFF)
#else /* PLATFORM_USES_LITTLE_ENDIAN */
        : A(0xFF), R(static_cast<u8>(InColor * 255.0)), G(static_cast<u8>(InColor * 255.0)), B(static_cast<u8>(InColor * 255.0))
#endif /* !PLATFORM_USES_LITTLE_ENDIAN */
    { }
    FORCEINLINE constexpr          LColor(const LColor& InColor) : Bits(InColor.Bits) { }
    FORCEINLINE constexpr explicit LColor(const u32 InColor)  : Bits(InColor)      { }
    FORCEINLINE constexpr explicit LColor(u8 InChannels[4])
        : Channels{ InChannels[0], InChannels[1], InChannels[2], InChannels[3] }      { }

    FORCEINLINE void operator=(const LColor& Other)      { this->Bits = Other.Bits; }
    FORCEINLINE void operator=(const u32 InColor)     { this->Bits = InColor;    }
    FORCEINLINE void operator=(const u8 InChannels[])
    {
        this->Channels[0] = InChannels[0];
        this->Channels[1] = InChannels[1];
        this->Channels[2] = InChannels[2];
        this->Channels[3] = InChannels[3];
    }

    FORCEINLINE u32 RGBA() const
    {
        return (static_cast<u32>(R) << 24) | (static_cast<u32>(G) << 16) | (static_cast<u32>(B) << 8) | static_cast<u32>(A);
    }
    FORCEINLINE u32 BGRA() const
    {
        return (static_cast<u32>(B) << 24) | (static_cast<u32>(G) << 16) | (static_cast<u32>(R) << 8) | static_cast<u32>(A);
    }

    FORCEINLINE LVector ToVector3() const
    {
        return LVector(this->R / 255.0f, this->G / 255.0f, this->B / 255.0f);
    }

    FORCEINLINE LVector4 ToVector4() const
    {
        return LVector4(this->R / 255.0f, this->G / 255.0f, this->B / 255.0f, this->A / 255.0f);
    }

    FORCEINLINE bool operator==(const LColor& Other) const { return Bits == Other.Bits; }
    FORCEINLINE bool operator!=(const LColor& Other) const { return Bits != Other.Bits; }

    FORCEINLINE void operator+=(const LColor& Other)
    {
        R += static_cast<u8>(Maths::Min(static_cast<u32>(R) + static_cast<u32>(Other.R), 0xFFu));
        G += static_cast<u8>(Maths::Min(static_cast<u32>(R) + static_cast<u32>(Other.R), 0xFFu));
        B += static_cast<u8>(Maths::Min(static_cast<u32>(R) + static_cast<u32>(Other.R), 0xFFu));
        A += static_cast<u8>(Maths::Min(static_cast<u32>(R) + static_cast<u32>(Other.R), 0xFFu));
    }
    FORCEINLINE void operator-=(const LColor& Other)
    {
        R -= static_cast<u8>(Maths::Max(static_cast<i32>(R) - static_cast<i32>(Other.R), 0x00));
        G -= static_cast<u8>(Maths::Max(static_cast<i32>(G) - static_cast<i32>(Other.G), 0x00));
        B -= static_cast<u8>(Maths::Max(static_cast<i32>(B) - static_cast<i32>(Other.B), 0x00));
        A -= static_cast<u8>(Maths::Max(static_cast<i32>(A) - static_cast<i32>(Other.A), 0x00));
    }

    static const LColor Transparent;
    static const LColor White;
    static const LColor Black;
    static const LColor Gray;
    static const LColor Red;
    static const LColor DarkRed;
    static const LColor Green;
    static const LColor Emerald;
    static const LColor Blue;
    static const LColor Yellow;
    static const LColor Cyan;
    static const LColor Magenta;

    FORCEINLINE LSimpleString ToString() const
    {
        return LSimpleString::SprintF("RGBA: {} {} {} {}", this->R, this->G, this->B, this->A);
    }
};

} /* ~Namespace Jafg */
