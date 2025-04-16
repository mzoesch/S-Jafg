// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Platform/PlatformMisc.h"

namespace Jafg
{

ENGINE_API LPlatformMisc* GPlatformMisc = nullptr;

LPath PlatformMisc::GetEngineRootDir()
{
#if WITH_VIRTUAL_FILESYSTEM
    LOG_WARNING(LogSystem, "Access to the filesystem is denied on this platform. Tried to access: [ENGINE_ROOT_DIR].")
    return "";
#else /* WITH_VIRTUAL_FILESYSTEM */
    checkSlow( GPlatformMisc )
    if (GPlatformMisc->EngineRootDir.IsEmpty())
    {
        GPlatformMisc->EngineRootDir = PlatformMisc::GetEngineRootDirImpl();
    }
    check( GPlatformMisc->EngineRootDir.IsEmpty() == false )
    return GPlatformMisc->EngineRootDir;
#endif /* !WITH_VIRTUAL_FILESYSTEM */
}

LPath PlatformMisc::GetRealEngineRootDir()
{
#if WITH_VIRTUAL_FILESYSTEM
    LOG_WARNING(LogSystem, "Access to the filesystem is denied on this platform. Tried to access: [REAL_ENGINE_ROOT_DIR].")
    return "";
#else /* WITH_VIRTUAL_FILESYSTEM */
    checkSlow( GPlatformMisc )
    if (GPlatformMisc->RealEngineRootDir.IsEmpty())
    {
        GPlatformMisc->RealEngineRootDir = PlatformMisc::GetRealEngineRootDirImpl();
    }
    check( GPlatformMisc->RealEngineRootDir.IsEmpty() == false )
    return GPlatformMisc->RealEngineRootDir;
#endif /* !WITH_VIRTUAL_FILESYSTEM */
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
