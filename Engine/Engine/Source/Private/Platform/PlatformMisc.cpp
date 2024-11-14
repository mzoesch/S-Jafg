// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Platform/PlatformMisc.h"
#include "System/Path.h"

namespace Jafg
{

ENGINE_API LPlatformMisc* GPlatformMisc = nullptr;

LSimpleString PlatformMisc::GetEngineRootDir()
{
    if (GPlatformMisc->EngineRootDir.IsEmpty())
    {
        GPlatformMisc->EngineRootDir = PlatformMisc::GetEngineRootDirImpl();
    }

    check( GPlatformMisc->EngineRootDir.IsEmpty() == false )

    return GPlatformMisc->EngineRootDir;
}

LSimpleString PlatformMisc::GetRealEngineRootDir()
{
    if (GPlatformMisc->RealEngineRootDir.IsEmpty())
    {
        GPlatformMisc->RealEngineRootDir = PlatformMisc::GetRealEngineRootDirImpl();
    }

    check( GPlatformMisc->RealEngineRootDir.IsEmpty() == false )

    return GPlatformMisc->RealEngineRootDir;
}

void PlatformMisc::InvalidateCachedValues()
{
    delete GPlatformMisc;
    GPlatformMisc = new LPlatformMisc();
    GPlatformMisc->MakeCachedValuesValid();
    checkSlow( GPlatformMisc )

    return;
}

void LPlatformMisc::MakeCachedValuesValid()
{
    this->NumberOfPhysicalViewports = PlatformMisc::GetNumberOfPhysicalViewports();
    if (PlatformMisc::SetPhysicalViewports() == false)
    {
        LOG_ERROR(LogPlatformMisc, "Failed to get physical viewports.")
    }

    return;
}

} /* ~Namespace Jafg */
