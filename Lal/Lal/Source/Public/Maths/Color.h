// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

struct LLinearColor;
struct LColor;

/**
 * Stores a color of BGRA format with 32 bits of precision per channel.
 */
struct LLinearColor final
{
#if PLATFORM_USES_LITTLE_ENDIAN
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
#else /* PLATFORM_USES_LITTLE_ENDIAN */
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
#endif /* !PLATFORM_USES_LITTLE_ENDIAN */

    FORCEINLINE constexpr LLinearColor() noexcept
#if PLATFORM_USES_LITTLE_ENDIAN
        : B(0.0f), G(0.0f), R(0.0f), A(0.0f)
#else /* PLATFORM_USES_LITTLE_ENDIAN */
        : A(0.0f), R(0.0f), G(0.0f), B(0.0f)
#endif /* !PLATFORM_USES_LITTLE_ENDIAN */
    {
    }

    FORCEINLINE constexpr LLinearColor(const f32 InR, const f32 InG, const f32 InB, const f32 InA = 1.0f) noexcept
#if PLATFORM_USES_LITTLE_ENDIAN
        : B(InB), G(InG), R(InR), A(InA)
#else /* PLATFORM_USES_LITTLE_ENDIAN */
        : A(InA), R(InR), G(InG), B(InB)
#endif /* !PLATFORM_USES_LITTLE_ENDIAN */
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
#if PLATFORM_USES_LITTLE_ENDIAN
        this->B = InColor.B;
        this->G = InColor.G;
        this->R = InColor.R;
        this->A = InColor.A;
#else /* PLATFORM_USES_LITTLE_ENDIAN */
        this->A = InColor.A;
        this->R = InColor.R;
        this->G = InColor.G;
        this->B = InColor.B;
#endif /* !PLATFORM_USES_LITTLE_ENDIAN */
    }

    FORCEINLINE constexpr LVector ToVector3() const noexcept
    {
        return {this->R, this->G, this->B};
    }

    FORCEINLINE constexpr LVector4 ToVector4() const noexcept
    {
        return {this->R, this->G, this->B, this->A};
    }

    FORCEINLINE static constexpr LLinearColor FromColor(const LColor& InColor) noexcept;
    FORCEINLINE static constexpr LLinearColor FromColor(const u8 InR, const u8 InG, const u8 InB, const u8 InA) noexcept;
    FORCEINLINE        constexpr LColor       ToColor() const noexcept;

    FORCEINLINE bool operator==(const LLinearColor& Other) const noexcept { return ::memcmp(this, &Other, sizeof(LLinearColor)) == 0; }
    FORCEINLINE bool operator!=(const LLinearColor& Other) const noexcept { return ::memcmp(this, &Other, sizeof(LLinearColor)) != 0; }

    FORCEINLINE constexpr void operator+=(const LLinearColor& Other) noexcept
    {
        this->R += Maths::Min(this->R + Other.R, 1.0f);
        this->G += Maths::Min(this->R + Other.R, 1.0f);
        this->B += Maths::Min(this->R + Other.R, 1.0f);
        this->A += Maths::Min(this->R + Other.R, 1.0f);

        return;
    }
    FORCEINLINE constexpr void operator-=(const LLinearColor& Other) noexcept
    {
        this->R -= Maths::Max(this->R - Other.R, 0.0f);
        this->G -= Maths::Max(this->G - Other.G, 0.0f);
        this->B -= Maths::Max(this->B - Other.B, 0.0f);
        this->A -= Maths::Max(this->A - Other.A, 0.0f);

        return;
    }

    FORCEINLINE LString ToString() const
    {
        return LString::SprintF("RGBA: {} {} {} {}", this->R, this->G, this->B, this->A);
    }

    //#
    //# @see #LColor for some documentation on the colors below.
    //#
    LAL_API static const LLinearColor Aliceblue;
    LAL_API static const LLinearColor Antiquewhite;
    LAL_API static const LLinearColor Aqua;
    LAL_API static const LLinearColor Aquamarine;
    LAL_API static const LLinearColor Azure;
    LAL_API static const LLinearColor Beige;
    LAL_API static const LLinearColor Bisque;
    LAL_API static const LLinearColor Black;
    LAL_API static const LLinearColor Blanchedalmond;
    LAL_API static const LLinearColor Blue;
    LAL_API static const LLinearColor BlueViolet;
    LAL_API static const LLinearColor Brown;
    LAL_API static const LLinearColor BurlyWood;
    LAL_API static const LLinearColor CadetBlue;
    LAL_API static const LLinearColor Chartreuse;
    LAL_API static const LLinearColor Chocolate;
    LAL_API static const LLinearColor Coral;
    LAL_API static const LLinearColor CornflowerBlue;
    LAL_API static const LLinearColor Cornsilk;
    LAL_API static const LLinearColor Crimson;
    LAL_API static const LLinearColor Cyan;
    LAL_API static const LLinearColor DarkBlue;
    LAL_API static const LLinearColor DarkCyan;
    LAL_API static const LLinearColor DarkerGray;
    LAL_API static const LLinearColor DarkerGrey;
    LAL_API static const LLinearColor DarkGoldenRod;
    LAL_API static const LLinearColor DarkGray;
    LAL_API static const LLinearColor DarkGreen;
    LAL_API static const LLinearColor DarkGrey;
    LAL_API static const LLinearColor DarkKhaki;
    LAL_API static const LLinearColor DarkMagenta;
    LAL_API static const LLinearColor DarkOliveGreen;
    LAL_API static const LLinearColor DarkOrange;
    LAL_API static const LLinearColor DarkOrchid;
    LAL_API static const LLinearColor DarkRed;
    LAL_API static const LLinearColor DarkSalmon;
    LAL_API static const LLinearColor DarkSeaGreen;
    LAL_API static const LLinearColor DarkSlateBlue;
    LAL_API static const LLinearColor DarkSlateGray;
    LAL_API static const LLinearColor DarkSlateGrey;
    LAL_API static const LLinearColor DarkTurquoise;
    LAL_API static const LLinearColor DarkViolet;
    LAL_API static const LLinearColor DeepPink;
    LAL_API static const LLinearColor DeepSkyBlue;
    LAL_API static const LLinearColor DimGray;
    LAL_API static const LLinearColor DimGrey;
    LAL_API static const LLinearColor DodgerBlue;
    LAL_API static const LLinearColor FireBrick;
    LAL_API static const LLinearColor FloralWhite;
    LAL_API static const LLinearColor ForestGreen;
    LAL_API static const LLinearColor Fuchsia;
    LAL_API static const LLinearColor Gainsboro;
    LAL_API static const LLinearColor GhostWhite;
    LAL_API static const LLinearColor Gold;
    LAL_API static const LLinearColor Goldenrod;
    LAL_API static const LLinearColor Gray;
    LAL_API static const LLinearColor Green;
    LAL_API static const LLinearColor GreenYellow;
    LAL_API static const LLinearColor Grey;
    LAL_API static const LLinearColor Honeydew;
    LAL_API static const LLinearColor HotPink;
    LAL_API static const LLinearColor Indianred;
    LAL_API static const LLinearColor Indigo;
    LAL_API static const LLinearColor Ivory;
    LAL_API static const LLinearColor Khaki;
    LAL_API static const LLinearColor Lavender;
    LAL_API static const LLinearColor LavenderBlush;
    LAL_API static const LLinearColor LawnGreen;
    LAL_API static const LLinearColor LemonChiffon;
    LAL_API static const LLinearColor LightBlue;
    LAL_API static const LLinearColor LightCoral;
    LAL_API static const LLinearColor LightCyan;
    LAL_API static const LLinearColor LightGoldenrodYellow;
    LAL_API static const LLinearColor LightGray;
    LAL_API static const LLinearColor LightGreen;
    LAL_API static const LLinearColor LightGrey;
    LAL_API static const LLinearColor LightPink;
    LAL_API static const LLinearColor LightSalmon;
    LAL_API static const LLinearColor LightSeagreen;
    LAL_API static const LLinearColor LightSkyBlue;
    LAL_API static const LLinearColor LightSlateGray;
    LAL_API static const LLinearColor LightSlateGrey;
    LAL_API static const LLinearColor LightSteelBlue;
    LAL_API static const LLinearColor LightYellow;
    LAL_API static const LLinearColor Lime;
    LAL_API static const LLinearColor LimeGreen;
    LAL_API static const LLinearColor Linen;
    LAL_API static const LLinearColor Magenta;
    LAL_API static const LLinearColor Maroon;
    LAL_API static const LLinearColor MediumAquaMarine;
    LAL_API static const LLinearColor MediumBlue;
    LAL_API static const LLinearColor MediumOrchid;
    LAL_API static const LLinearColor MediumPurple;
    LAL_API static const LLinearColor MediumSeagreen;
    LAL_API static const LLinearColor MediumSlateBlue;
    LAL_API static const LLinearColor MediumSpringGreen;
    LAL_API static const LLinearColor MediumTurquoise;
    LAL_API static const LLinearColor MediumVioletRed;
    LAL_API static const LLinearColor MidnightBlue;
    LAL_API static const LLinearColor MintCream;
    LAL_API static const LLinearColor Mistyrose;
    LAL_API static const LLinearColor Moccasin;
    LAL_API static const LLinearColor NavajoWhite;
    LAL_API static const LLinearColor Navy;
    LAL_API static const LLinearColor NotSoDarkGray;
    LAL_API static const LLinearColor NotSoDarkGrey;
    LAL_API static const LLinearColor Oldlace;
    LAL_API static const LLinearColor Olive;
    LAL_API static const LLinearColor Olivedrab;
    LAL_API static const LLinearColor Orange;
    LAL_API static const LLinearColor Orangered;
    LAL_API static const LLinearColor Orchid;
    LAL_API static const LLinearColor PaleGoldenRod;
    LAL_API static const LLinearColor PaleGreen;
    LAL_API static const LLinearColor PaleTurquoise;
    LAL_API static const LLinearColor PaleVioletRed;
    LAL_API static const LLinearColor Papayawhip;
    LAL_API static const LLinearColor Peachpuff;
    LAL_API static const LLinearColor Peru;
    LAL_API static const LLinearColor Pink;
    LAL_API static const LLinearColor Plum;
    LAL_API static const LLinearColor PowderBlue;
    LAL_API static const LLinearColor Purple;
    LAL_API static const LLinearColor RebeccaPurple;
    LAL_API static const LLinearColor Red;
    LAL_API static const LLinearColor RosyBrown;
    LAL_API static const LLinearColor RoyalBlue;
    LAL_API static const LLinearColor SaddleBrown;
    LAL_API static const LLinearColor Salmon;
    LAL_API static const LLinearColor SandyBrown;
    LAL_API static const LLinearColor SeaGreen;
    LAL_API static const LLinearColor Seashell;
    LAL_API static const LLinearColor Sienna;
    LAL_API static const LLinearColor Silver;
    LAL_API static const LLinearColor SkyBlue;
    LAL_API static const LLinearColor SlateBlue;
    LAL_API static const LLinearColor SlateGray;
    LAL_API static const LLinearColor SlateGrey;
    LAL_API static const LLinearColor Snow;
    LAL_API static const LLinearColor SpringGreen;
    LAL_API static const LLinearColor SteelBlue;
    LAL_API static const LLinearColor Tan;
    LAL_API static const LLinearColor Teal;
    LAL_API static const LLinearColor Thistle;
    LAL_API static const LLinearColor Tomato;
    LAL_API static const LLinearColor Transparent;
    LAL_API static const LLinearColor Turquoise;
    LAL_API static const LLinearColor Violet;
    LAL_API static const LLinearColor Wheat;
    LAL_API static const LLinearColor White;
    LAL_API static const LLinearColor WhiteSmoke;
    LAL_API static const LLinearColor Yellow;
    LAL_API static const LLinearColor YellowGreen;
};

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
    {
    }

    FORCEINLINE constexpr LColor(const u8 InR, const u8 InG, const u8 InB, const u8 InA = 0xFF) noexcept
#if PLATFORM_USES_LITTLE_ENDIAN
        : B(InB), G(InG), R(InR), A(InA)
#else /* PLATFORM_USES_LITTLE_ENDIAN */
        : A(InA), R(InR), G(InG), B(InB)
#endif /* !PLATFORM_USES_LITTLE_ENDIAN */
    {
    }

    FORCEINLINE constexpr LColor(const u8 InColor) noexcept
#if PLATFORM_USES_LITTLE_ENDIAN
        : B(InColor), G(InColor), R(InColor), A(0xFF)
#else /* PLATFORM_USES_LITTLE_ENDIAN */
        : A(InA), R(InR), G(InG), B(InB)
#endif /* !PLATFORM_USES_LITTLE_ENDIAN */
    {
    }

    FORCEINLINE constexpr LColor(const u32 InColor) noexcept
#if PLATFORM_USES_LITTLE_ENDIAN
        : B(static_cast<u8>(InColor)), G(static_cast<u8>(InColor)), R(static_cast<u8>(InColor)), A(0xFF)
#else /* PLATFORM_USES_LITTLE_ENDIAN */
        : A(InA), R(InR), G(InG), B(InB)
#endif /* !PLATFORM_USES_LITTLE_ENDIAN */
    {
    }

    FORCEINLINE constexpr explicit LColor(const f32 InColor) noexcept
#if PLATFORM_USES_LITTLE_ENDIAN
        : B(static_cast<u8>(InColor * 255.0f)), G(static_cast<u8>(InColor * 255.0f)), R(static_cast<u8>(InColor * 255.0f)), A(0xFF)
#else /* PLATFORM_USES_LITTLE_ENDIAN */
        : A(0xFF), R(static_cast<u8>(InColor * 255.0f)), G(static_cast<u8>(InColor * 255.0f)), B(static_cast<u8>(InColor * 255.0f))
#endif /* !PLATFORM_USES_LITTLE_ENDIAN */
    {
    }

    FORCEINLINE constexpr explicit LColor(const f64 InColor) noexcept
#if PLATFORM_USES_LITTLE_ENDIAN
        : B(static_cast<u8>(InColor * 255.0)), G(static_cast<u8>(InColor * 255.0)), R(static_cast<u8>(InColor * 255.0)), A(0xFF)
#else /* PLATFORM_USES_LITTLE_ENDIAN */
        : A(0xFF), R(static_cast<u8>(InColor * 255.0)), G(static_cast<u8>(InColor * 255.0)), B(static_cast<u8>(InColor * 255.0))
#endif /* !PLATFORM_USES_LITTLE_ENDIAN */
    {
    }

    FORCEINLINE constexpr LColor(const LColor& InColor) noexcept
    : Bits(InColor.Bits)
    {
    }

    FORCEINLINE constexpr explicit LColor(u8 InChannels[4]) noexcept
        : Channels{ InChannels[0], InChannels[1], InChannels[2], InChannels[3] }
    {
    }

    FORCEINLINE constexpr ~LColor() noexcept = default;

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

    FORCEINLINE static constexpr LColor FromLinearColor(const LLinearColor& InColor) noexcept;
    FORCEINLINE constexpr LLinearColor ToLinearColor() const noexcept;

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

    FORCEINLINE LString ToString() const
    {
        return LString::SprintF("RGBA: {} {} {} {}", this->R, this->G, this->B, this->A);
    }

    //#
    //# The following colors do not follow any CSS standard. And are just part here for
    //# missing color values in the standard.
    //#

    LAL_API static const LColor DarkerGray;
    LAL_API static const LColor DarkerGrey;
    LAL_API static const LColor NotSoDarkGray;
    LAL_API static const LColor NotSoDarkGrey;

    //#
    //# The following colors follow the CSS standard.
    //# https://developer.mozilla.org/en-US/docs/Web/CSS/named-color
    //#

    LAL_API static const LColor Transparent;

    //#
    //# BEGIN Standard
    //#

    LAL_API static const LColor Black;
    LAL_API static const LColor Silver;
    LAL_API static const LColor Gray;
    LAL_API static const LColor White;
    LAL_API static const LColor Maroon;
    LAL_API static const LColor Red;
    LAL_API static const LColor Purple;
    LAL_API static const LColor Fuchsia;
    LAL_API static const LColor Green;
    LAL_API static const LColor Lime;
    LAL_API static const LColor Olive;
    LAL_API static const LColor Yellow;
    LAL_API static const LColor Navy;
    LAL_API static const LColor Blue;
    LAL_API static const LColor Teal;
    LAL_API static const LColor Aqua;

    //#
    //# BEGIN Additional Standard
    //#

    LAL_API static const LColor Aliceblue;
    LAL_API static const LColor Antiquewhite;
    // LAL_API static const LColor Aqua;
    LAL_API static const LColor Aquamarine;
    LAL_API static const LColor Azure;
    LAL_API static const LColor Beige;
    LAL_API static const LColor Bisque;
    // LAL_API static const LColor Black;
    LAL_API static const LColor Blanchedalmond;
    // LAL_API static const LColor Blue;
    LAL_API static const LColor BlueViolet;
    LAL_API static const LColor Brown;
    LAL_API static const LColor BurlyWood;
    LAL_API static const LColor CadetBlue;
    LAL_API static const LColor Chartreuse;
    LAL_API static const LColor Chocolate;
    LAL_API static const LColor Coral;
    LAL_API static const LColor CornflowerBlue;
    LAL_API static const LColor Cornsilk;
    LAL_API static const LColor Crimson;
    LAL_API static const LColor Cyan;
    LAL_API static const LColor DarkBlue;
    LAL_API static const LColor DarkCyan;
    LAL_API static const LColor DarkGoldenRod;
    LAL_API static const LColor DarkGray;
    LAL_API static const LColor DarkGreen;
    LAL_API static const LColor DarkGrey;
    LAL_API static const LColor DarkKhaki;
    LAL_API static const LColor DarkMagenta;
    LAL_API static const LColor DarkOliveGreen;
    LAL_API static const LColor DarkOrange;
    LAL_API static const LColor DarkOrchid;
    LAL_API static const LColor DarkRed;
    LAL_API static const LColor DarkSalmon;
    LAL_API static const LColor DarkSeaGreen;
    LAL_API static const LColor DarkSlateBlue;
    LAL_API static const LColor DarkSlateGray;
    LAL_API static const LColor DarkSlateGrey;
    LAL_API static const LColor DarkTurquoise;
    LAL_API static const LColor DarkViolet;
    LAL_API static const LColor DeepPink;
    LAL_API static const LColor DeepSkyBlue;
    LAL_API static const LColor DimGray;
    LAL_API static const LColor DimGrey;
    LAL_API static const LColor DodgerBlue;
    LAL_API static const LColor FireBrick;
    LAL_API static const LColor FloralWhite;
    LAL_API static const LColor ForestGreen;
    // LAL_API static const LColor Fuchsia;
    LAL_API static const LColor Gainsboro;
    LAL_API static const LColor GhostWhite;
    LAL_API static const LColor Gold;
    LAL_API static const LColor Goldenrod;
    // LAL_API static const LColor Gray;
    // LAL_API static const LColor Green;
    LAL_API static const LColor GreenYellow;
    LAL_API static const LColor Grey;
    LAL_API static const LColor Honeydew;
    LAL_API static const LColor HotPink;
    LAL_API static const LColor Indianred;
    LAL_API static const LColor Indigo;
    LAL_API static const LColor Ivory;
    LAL_API static const LColor Khaki;
    LAL_API static const LColor Lavender;
    LAL_API static const LColor LavenderBlush;
    LAL_API static const LColor LawnGreen;
    LAL_API static const LColor LemonChiffon;
    LAL_API static const LColor LightBlue;
    LAL_API static const LColor LightCoral;
    LAL_API static const LColor LightCyan;
    LAL_API static const LColor LightGoldenrodYellow;
    LAL_API static const LColor LightGray;
    LAL_API static const LColor LightGreen;
    LAL_API static const LColor LightGrey;
    LAL_API static const LColor LightPink;
    LAL_API static const LColor LightSalmon;
    LAL_API static const LColor LightSeagreen;
    LAL_API static const LColor LightSkyBlue;
    LAL_API static const LColor LightSlateGray;
    LAL_API static const LColor LightSlateGrey;
    LAL_API static const LColor LightSteelBlue;
    LAL_API static const LColor LightYellow;
    // LAL_API static const LColor Lime;
    LAL_API static const LColor LimeGreen;
    LAL_API static const LColor Linen;
    LAL_API static const LColor Magenta;
    // LAL_API static const LColor Maroon;
    LAL_API static const LColor MediumAquaMarine;
    LAL_API static const LColor MediumBlue;
    LAL_API static const LColor MediumOrchid;
    LAL_API static const LColor MediumPurple;
    LAL_API static const LColor MediumSeagreen;
    LAL_API static const LColor MediumSlateBlue;
    LAL_API static const LColor MediumSpringGreen;
    LAL_API static const LColor MediumTurquoise;
    LAL_API static const LColor MediumVioletRed;
    LAL_API static const LColor MidnightBlue;
    LAL_API static const LColor MintCream;
    LAL_API static const LColor Mistyrose;
    LAL_API static const LColor Moccasin;
    LAL_API static const LColor NavajoWhite;
    // LAL_API static const LColor Navy;
    LAL_API static const LColor Oldlace;
    // LAL_API static const LColor Olive;
    LAL_API static const LColor Olivedrab;
    LAL_API static const LColor Orange;
    LAL_API static const LColor Orangered;
    LAL_API static const LColor Orchid;
    LAL_API static const LColor PaleGoldenRod;
    LAL_API static const LColor PaleGreen;
    LAL_API static const LColor PaleTurquoise;
    LAL_API static const LColor PaleVioletRed;
    LAL_API static const LColor Papayawhip;
    LAL_API static const LColor Peachpuff;
    LAL_API static const LColor Peru;
    LAL_API static const LColor Pink;
    LAL_API static const LColor Plum;
    LAL_API static const LColor PowderBlue;
    // LAL_API static const LColor Purple;
    LAL_API static const LColor RebeccaPurple;
    // LAL_API static const LColor Red;
    LAL_API static const LColor RosyBrown;
    LAL_API static const LColor RoyalBlue;
    LAL_API static const LColor SaddleBrown;
    LAL_API static const LColor Salmon;
    LAL_API static const LColor SandyBrown;
    LAL_API static const LColor SeaGreen;
    LAL_API static const LColor Seashell;
    LAL_API static const LColor Sienna;
    // LAL_API static const LColor Silver;
    LAL_API static const LColor SkyBlue;
    LAL_API static const LColor SlateBlue;
    LAL_API static const LColor SlateGray;
    LAL_API static const LColor SlateGrey;
    LAL_API static const LColor Snow;
    LAL_API static const LColor SpringGreen;
    LAL_API static const LColor SteelBlue;
    LAL_API static const LColor Tan;
    // LAL_API static const LColor Teal;
    LAL_API static const LColor Thistle;
    LAL_API static const LColor Tomato;
    // LAL_API static const LColor Transparent;
    LAL_API static const LColor Turquoise;
    LAL_API static const LColor Violet;
    LAL_API static const LColor Wheat;
    // LAL_API static const LColor White;
    LAL_API static const LColor WhiteSmoke;
    // LAL_API static const LColor Yellow;
    LAL_API static const LColor YellowGreen;
};

FORCEINLINE constexpr LLinearColor LLinearColor::FromColor(const LColor& InColor) noexcept
{
    return
    {
        InColor.R / 255.0f,
        InColor.G / 255.0f,
        InColor.B / 255.0f,
        InColor.A / 255.0f,
    };
}

FORCEINLINE constexpr LLinearColor LLinearColor::FromColor(const u8 InR, const u8 InG, const u8 InB, const u8 InA) noexcept
{
    return
    {
        static_cast<f32>(InR) / 255.0f,
        static_cast<f32>(InG) / 255.0f,
        static_cast<f32>(InB) / 255.0f,
        static_cast<f32>(InA) / 255.0f,
    };
}

FORCEINLINE constexpr LColor LLinearColor::ToColor() const noexcept
{
    return
    {
        static_cast<u8>(this->R * 255.0f),
        static_cast<u8>(this->G * 255.0f),
        static_cast<u8>(this->B * 255.0f),
        static_cast<u8>(this->A * 255.0f),
    };
}

FORCEINLINE constexpr LColor LColor::FromLinearColor(const LLinearColor& InColor) noexcept
{
    return
    {
        static_cast<u8>(InColor.R * 255.0f),
        static_cast<u8>(InColor.G * 255.0f),
        static_cast<u8>(InColor.B * 255.0f),
        static_cast<u8>(InColor.A * 255.0f),
    };
}

FORCEINLINE constexpr LLinearColor LColor::ToLinearColor() const noexcept
{
    return
    {
        this->R / 255.0f,
        this->G / 255.0f,
        this->B / 255.0f,
        this->A / 255.0f,
    };
}

} /* ~Namespace Jafg */
