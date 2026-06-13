// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

class LFrontendBase;

#if JAFG_PLATFORM_DESKTOP
    class LFrontendNativeDesktop;
#endif /* JAFG_PLATFORM_DESKTOP */

class LFrontendVk;

typedef LFrontendVk LFrontend;

//# Represents an external physical monitor.
struct LPhysicalViewport final
{
    void* Handle{};

    // Physical size in millimeters.
    LVec2i32 SizeMm;
    // Virtual size in pixels.
    LVec2i32 WorkareaPx;
    // Offset of the workarea in pixels.
    LVec2i32 WorkareaOffsetPx;

    // Scale of content.
    LVec2F ContentScale;

    //# If multiple monitors of the same name exist, this prefix can help to distinguish them.
    std::size_t Index;
    //# Name of the monitor.
    LString Name;

    //# Whether this is the primary monitor.
    //# ??? Just fucking ignore that. For windows yay; for x11 yay; for wayland hell nah.
    // bool bPrimary{ false };

    struct VideoMode final
    {
        //# In this order: RGB.
        LVec3i32 Bits;
        LVec2i32 ResolutionPx;
        i32 RefreshRateHz;

        NODISCARD FORCEINLINE LString ToHumanReadableString() const noexcept
        {
            return algo::sprintf("{}bpp {}x{}px @ {}hz",
                this->Bits.x + this->Bits.y + this->Bits.z,
                this->ResolutionPx.x, this->ResolutionPx.y,
                this->RefreshRateHz
                );
        }
    };
    TArray<VideoMode> VideoModes;

    VideoMode CurrentVideoMode;

    NODISCARD FORCEINLINE LString ToHumanReadableName() const noexcept
    {
        return algo::sprintf("{}-{} ({}x{}px)",
            this->Index,
            this->Name,
            this->WorkareaPx.x, this->WorkareaPx.y
            );
    }
};

} /* ~Namespace Jafg */
