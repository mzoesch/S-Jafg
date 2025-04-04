// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

struct LPlatformMisc;

//#
//# Represents an external physical monitor.
//#
struct LPhysicalViewport
{
    //# Physical width in mm.
    i32 WidthMm  = 0;

    //# Physical height in mm.
    i32 HeightMm = 0;

    //# Virtual width in pixels.
    i32 WidthPx  = 0;

    //# Virtual height in pixels.
    i32 HeightPx = 0;

    //# Name of the monitor.
    LString Name;

    FORCEINLINE LString ToString() const
    {
        return LString::SprintF("Monitor{{{},{}x{}mm,{}x{}px}}",
            this->Name,
            this->WidthMm, this->HeightMm,
            this->WidthPx, this->HeightPx
        );
    }
};

namespace PlatformMisc
{

//#
//# The engine root dir that is currently being used. This might not be the real engine root dir as the runtime
//# console application might not be inside this directory.
//# In shipped builds, this is the same as GetRealEngineRootDir().
//#
ENGINE_API LString GetEngineRootDir();
ENGINE_API LString GetEngineRootDirImpl();

//#
//# The real engine root dir where the runtime console application is located and running from.
//# In shipped builds, this is the same as GetEngineRootDir().
//#
ENGINE_API LString GetRealEngineRootDir();
ENGINE_API LString GetRealEngineRootDirImpl();

//#
//# Invalidate all cached values and reinitialize them inside GPlatformMisc.
//#
ENGINE_API void InvalidateCachedValues();

//#
//# The number of physical viewports available on the current platform.
//# INDEX_NONE if an error occurred.
//#
ENGINE_API auto GetNumberOfPhysicalViewports() -> i32;
ENGINE_API auto SetPhysicalViewports() -> bool;

} /* ~Namespace PlatformMisc */

ENGINE_API extern LPlatformMisc* GPlatformMisc;

//#
//# Cached values of platform-specific information.
//# Loaded at startup if values have to be invalidated run PlatformMisc::InvalidateCachedValues().
//#
struct ENGINE_API LPlatformMisc
{
    LPlatformMisc() = default;
    void MakeCachedValuesValid();

    i32 NumberOfPhysicalViewports = INDEX_NONE;
    TArray<LPhysicalViewport> PhysicalViewports;
    LString EngineRootDir;
    LString RealEngineRootDir;
};

} /* ~Namespace Jafg */
