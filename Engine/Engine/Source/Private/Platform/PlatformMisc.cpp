// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Platform/PlatformMisc.h"

namespace Jafg
{

ENGINE_API LPlatformMisc* GPlatformMisc = nullptr;

LString PlatformMisc::GetEngineRootDir()
{
#if  !WITH_VIRTUAL_FILESYSTEM
    if (GPlatformMisc->EngineRootDir.IsEmpty())
    {
        GPlatformMisc->EngineRootDir = PlatformMisc::GetEngineRootDirImpl();
    }
    check( GPlatformMisc->EngineRootDir.IsEmpty() == false )
#endif /* !WITH_VIRTUAL_FILESYSTEM */

    return GPlatformMisc->EngineRootDir;
}

LString PlatformMisc::GetRealEngineRootDir()
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
