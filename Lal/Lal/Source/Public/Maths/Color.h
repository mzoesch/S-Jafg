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
            uint8 B;
            uint8 G;
            uint8 R;
            uint8 A;
        };

        uint32 Bits = 0x00u;

        uint8  Channels[4];
    };
#else /* PLATFORM_USES_LITTLE_ENDIAN */
    union
    {
        struct
        {
            uint8 A;
            uint8 R;
            uint8 G;
            uint8 B;
        };

        uint32 Bits = 0x00u;

        uint8  Channels[4];
    };
#endif /* !PLATFORM_USES_LITTLE_ENDIAN */

    FORCEINLINE  LColor() { }
    FORCEINLINE ~LColor() { }

    FORCEINLINE constexpr explicit LColor(const uint8 InR, const uint8 InG, const uint8 InB, const uint8 InA = 0xFF)
#if PLATFORM_USES_LITTLE_ENDIAN
        : B(InB), G(InG), R(InR), A(InA)
#else /* PLATFORM_USES_LITTLE_ENDIAN */
        : A(InA), R(InR), G(InG), B(InB)
#endif /* !PLATFORM_USES_LITTLE_ENDIAN */
    { }

    FORCEINLINE constexpr          LColor(const LColor& InColor) : Bits(InColor.Bits) { }
    FORCEINLINE constexpr explicit LColor(const uint32 InColor)  : Bits(InColor)      { }
    FORCEINLINE constexpr explicit LColor(uint8 InChannels[4])
        : Channels{ InChannels[0], InChannels[1], InChannels[2], InChannels[3] }      { }

    FORCEINLINE void operator=(const LColor& Other)      { this->Bits = Other.Bits; }
    FORCEINLINE void operator=(const uint32 InColor)     { this->Bits = InColor;    }
    FORCEINLINE void operator=(const uint8 InChannels[])
    {
        this->Channels[0] = InChannels[0];
        this->Channels[1] = InChannels[1];
        this->Channels[2] = InChannels[2];
        this->Channels[3] = InChannels[3];
    }

    FORCEINLINE uint32 RGBA() const
    {
        return (static_cast<uint32>(R) << 24) | (static_cast<uint32>(G) << 16) | (static_cast<uint32>(B) << 8) | static_cast<uint32>(A);
    }
    FORCEINLINE uint32 BGRA() const
    {
        return (static_cast<uint32>(B) << 24) | (static_cast<uint32>(G) << 16) | (static_cast<uint32>(R) << 8) | static_cast<uint32>(A);
    }

    FORCEINLINE bool operator==(const LColor& Other) const { return Bits == Other.Bits; }
    FORCEINLINE bool operator!=(const LColor& Other) const { return Bits != Other.Bits; }

    FORCEINLINE void operator+=(const LColor& Other)
    {
        R += static_cast<uint8>(Maths::Min(static_cast<uint32>(R) + static_cast<uint32>(Other.R), 0xFFu));
        G += static_cast<uint8>(Maths::Min(static_cast<uint32>(R) + static_cast<uint32>(Other.R), 0xFFu));
        B += static_cast<uint8>(Maths::Min(static_cast<uint32>(R) + static_cast<uint32>(Other.R), 0xFFu));
        A += static_cast<uint8>(Maths::Min(static_cast<uint32>(R) + static_cast<uint32>(Other.R), 0xFFu));
    }
    FORCEINLINE void operator-=(const LColor& Other)
    {
        R -= static_cast<uint8>(Maths::Max(static_cast<int32>(R) - static_cast<int32>(Other.R), 0x00));
        G -= static_cast<uint8>(Maths::Max(static_cast<int32>(G) - static_cast<int32>(Other.G), 0x00));
        B -= static_cast<uint8>(Maths::Max(static_cast<int32>(B) - static_cast<int32>(Other.B), 0x00));
        A -= static_cast<uint8>(Maths::Max(static_cast<int32>(A) - static_cast<int32>(Other.A), 0x00));
    }

    static const LColor Transparent;
    static const LColor White;
    static const LColor Black;
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
        return LSimpleString::SprintF("RGBA: {} {} {} {}", R, G, B, A);
    }
};

} /* ~Namespace Jafg */
