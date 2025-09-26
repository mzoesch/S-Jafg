// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Jafg
{

struct LPlatformMisc;
struct LPhysicalViewport;

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
//#
ENGINE_API LPath GetEngineRootDir();

//#
//# The real engine root dir where the runtime console application is located and running from.
//#
ENGINE_API LPath GetRealEngineRootDir();

//# E.g.: "Windows", "Linux", ...
ENGINE_API LStringView GetTargetPlatform() noexcept;
//# E.g.: "x86_64", "x86", ...
ENGINE_API LStringView GetTargetArchitecture() noexcept;
//# E.g.: "Client", "Daemon", ...
ENGINE_API LStringView GetTargetType() noexcept;
//# E.g.: "Debug", "Shipping", ...
ENGINE_API LStringView GetTargetConfiguration() noexcept;
//# E.g.: "Client-Shipping", ...
ENGINE_API LStringView GetTargetCompound() noexcept;
//# E.g.: "Windows-x86_64", "Linux-x86_64", ...
ENGINE_API LStringView GetTargetPlatformCompound() noexcept;
//# E.g.: "Linux-x86_64/Client-Shipping", ...
ENGINE_API LStringView GetTargetConfigPath() noexcept;

inline LPath GetRootBinaryDirectory() noexcept { return LPath{ "Binaries" } / GetTargetConfigPath(); }

//#
//# The number of physical viewports available on the current platform.
//# INDEX_NONE if an error occurred.
//#
ENGINE_API i32  GetNumberOfPhysicalViewports();
ENGINE_API bool SetPhysicalViewports();

namespace Private
{

//#
//# Invalidate all cached values and reinitialize them inside GPlatformMisc.
//#
ENGINE_API void InvalidateCachedValues();

ENGINE_API LPath GetEngineRootDirImpl();
ENGINE_API LPath GetRealEngineRootDirImpl();

} /* ~Namespace Private */

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
    LPath EngineRootDir;
    LPath RealEngineRootDir;
};

} /* ~Namespace Jafg */
