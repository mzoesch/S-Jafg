// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Jafg
{

typedef u8 LLightDomainTy;
struct LLight final
{
    union
    {
        struct
        {
            LLightDomainTy R;
            LLightDomainTy G;
            LLightDomainTy B;
        };

        LLightDomainTy Channels[3];
    };

    FORCEINLINE constexpr LLight() : R(0), G(0), B(0) { }
    FORCEINLINE ~LLight() { }

    FORCEINLINE constexpr          LLight(const LLight& InLight) : R(InLight.R), G(InLight.G), B(InLight.B) { }
    FORCEINLINE constexpr explicit LLight(const LLightDomainTy InR, const LLightDomainTy InG, const LLightDomainTy InB)
        : R(InR), G(InG), B(InB) { }
    FORCEINLINE constexpr explicit LLight(const LLightDomainTy InChannels[3])
        : Channels{ InChannels[0], InChannels[1], InChannels[2] } { }
    FORCEINLINE constexpr explicit LLight(const u32 InColor) /* Does this work with big endian? */
        : R((InColor & 0x000000FF) >>  0), G((InColor & 0x0000FF00) >>  8), B((InColor & 0x00FF0000) >> 16) { }

    FORCEINLINE void operator=(const LLight& InLight) { this->R = InLight.R; this->G = InLight.G; this->B = InLight.B; }
    FORCEINLINE void operator=(const u32 InColor) { this->R = (InColor & 0x000000FF) >>  0; this->G = (InColor & 0x0000FF00) >>  8; this->B = (InColor & 0x00FF0000) >> 16; }
    FORCEINLINE void operator=(const LLightDomainTy InChannels[3]) { this->R = InChannels[0]; this->G = InChannels[1]; this->B = InChannels[2]; }

    FORCEINLINE u32 ToRGB() const { return (this->R <<  0) | (this->G <<  8) | (this->B << 16); }
    FORCEINLINE u32 ToBGR() const { return (this->B <<  0) | (this->G <<  8) | (this->R << 16); }
    FORCEINLINE void SetRGB(const LLightDomainTy InR, const LLightDomainTy InG, const LLightDomainTy InB) { this->R = InR; this->G = InG; this->B = InB; }
    FORCEINLINE void SetBGR(const LLightDomainTy InB, const LLightDomainTy InG, const LLightDomainTy InR) { this->B = InB; this->G = InG; this->R = InR; }
    FORCEINLINE void SetR(const LLightDomainTy InR) { this->R = InR; }
    FORCEINLINE void SetG(const LLightDomainTy InG) { this->G = InG; }
    FORCEINLINE void SetB(const LLightDomainTy InB) { this->B = InB; }

    FORCEINLINE bool operator==(const LLight& InLight) const { return this->R == InLight.R && this->G == InLight.G && this->B == InLight.B; }
    FORCEINLINE bool operator!=(const LLight& InLight) const { return this->R != InLight.R || this->G != InLight.G || this->B != InLight.B; }
    FORCEINLINE bool operator==(const u32 InColor) const { return this->R == ((InColor & 0x000000FF) >>  0) && this->G == ((InColor & 0x0000FF00) >>  8) && this->B == ((InColor & 0x00FF0000) >> 16); }
    FORCEINLINE bool operator!=(const u32 InColor) const { return this->R != ((InColor & 0x000000FF) >>  0) || this->G != ((InColor & 0x0000FF00) >>  8) || this->B != ((InColor & 0x00FF0000) >> 16); }

    FORCEINLINE void operator+=(const LLight& InLight)
    {
        this->R += static_cast<LLightDomainTy>(Maths::Min(static_cast<u32>(this->R) + static_cast<u32>(InLight.R), 0xFFu));
        this->G += static_cast<LLightDomainTy>(Maths::Min(static_cast<u32>(this->G) + static_cast<u32>(InLight.G), 0xFFu));
        this->B += static_cast<LLightDomainTy>(Maths::Min(static_cast<u32>(this->B) + static_cast<u32>(InLight.B), 0xFFu));
    }
    FORCEINLINE void operator-=(const LLight& InLight)
    {
        this->R -= static_cast<LLightDomainTy>(Maths::Max(static_cast<i32>(this->R) - static_cast<i32>(InLight.R), 0x00));
        this->G -= static_cast<LLightDomainTy>(Maths::Max(static_cast<i32>(this->G) - static_cast<i32>(InLight.G), 0x00));
        this->B -= static_cast<LLightDomainTy>(Maths::Max(static_cast<i32>(this->B) - static_cast<i32>(InLight.B), 0x00));
    }

    FORCEINLINE LSimpleString ToString() const
    {
        return LSimpleString::SprintF("RGB: {} {} {}", this->R, this->G, this->B);
    }
};

} /* ~Namespace Jafg */
