// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

DECLARE_INLINE_LOG_CATEGORY(LogPlatformMisc, Trace)

namespace Jafg
{

struct LPlatformMisc;

/**
 * Represents an external physical monitor.
 */
struct LPhysicalViewport
{
    /** Physical width in mm. */
    int32 WidthMm  = 0;

    /** Physical height in mm. */
    int32 HeightMm = 0;

    /** Virtual width in pixels. */
    int32 WidthPx  = 0;

    /** Virtual height in pixels. */
    int32 HeightPx = 0;

    /** Name of the monitor. */
    LSimpleString Name;

    FORCEINLINE LSimpleString ToString() const
    {
        return LSimpleString::SprintF("Monitor{{{},{}x{}mm,{}x{}px}}",
            this->Name,
            this->WidthMm, this->HeightMm,
            this->WidthPx, this->HeightPx
        );
    }
};

namespace PlatformMisc
{

/**
 * Invalidate all cached values and reinitialize them inside GPlatformMisc.
 */
RHI_API void InvalidateCachedValues();

/**
 * The number of physical viewports available on the current platform.
 * INDEX_NONE if an error occurred.
 */
RHI_API auto GetNumberOfPhysicalViewports() -> int32;
RHI_API auto SetPhysicalViewports() -> bool;

} /* ~Namespace PlatformMisc */

RHI_API extern LPlatformMisc* GPlatformMisc;

/**
 * Cached values of platform-specific information.
 * Loaded at startup if values have to be invalidated run PlatformMisc::InvalidateCachedValues().
 */
struct RHI_API LPlatformMisc
{
    LPlatformMisc() = default;
    void MakeCachedValuesValid();

    int32 NumberOfPhysicalViewports = INDEX_NONE;
    TdhArray<LPhysicalViewport> PhysicalViewports;
};

} /* ~Namespace Jafg */
