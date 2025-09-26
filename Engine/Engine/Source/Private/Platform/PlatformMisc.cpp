// Copyright mzoesch. All rights reserved.

#include "Platform/PlatformMisc.h"

namespace Jafg
{

ENGINE_API LPlatformMisc* GPlatformMisc { nullptr };

LPath PlatformMisc::GetEngineRootDir()
{
#if WITH_VIRTUAL_FILESYSTEM
    LOG_WARNING(LogSystem, "Access to the filesystem is denied on this platform. Tried to access: [ENGINE_ROOT_DIR].")
    return "";
#else /* WITH_VIRTUAL_FILESYSTEM */
    checkSlow( GPlatformMisc )
    if (GPlatformMisc->EngineRootDir.IsEmpty())
    {
        GPlatformMisc->EngineRootDir = PlatformMisc::Private::GetEngineRootDirImpl();
    }
    check( GPlatformMisc->EngineRootDir.IsEmpty() == false )
    check( Finder::DoesFileExist(GPlatformMisc->EngineRootDir / "jafg.jafgworkspace") )
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
        GPlatformMisc->RealEngineRootDir = PlatformMisc::Private::GetRealEngineRootDirImpl();
    }
    check( GPlatformMisc->RealEngineRootDir.IsEmpty() == false )
    return GPlatformMisc->RealEngineRootDir;
#endif /* !WITH_VIRTUAL_FILESYSTEM */
}

LStringView PlatformMisc::GetTargetPlatform() noexcept
{
    return LStringView{ PRIVATE_ENGINE_TARGET_PLATFORM };
}

LStringView PlatformMisc::GetTargetArchitecture() noexcept
{
    return LStringView{ PRIVATE_ENGINE_TARGET_ARCHITECTURE };
}

LStringView PlatformMisc::GetTargetType() noexcept
{
    return LStringView{ PRIVATE_ENGINE_TARGET_TYPE };
}

LStringView PlatformMisc::GetTargetConfiguration() noexcept
{
    return LStringView{ PRIVATE_ENGINE_TARGET_CONFIGURATION };
}

LStringView PlatformMisc::GetTargetCompound() noexcept
{
    return LStringView{ PRIVATE_ENGINE_TARGET_COMPOUND };
}

LStringView PlatformMisc::GetTargetPlatformCompound() noexcept
{
    return LStringView{ PRIVATE_ENGINE_PLATFORM_COMPOUND };
}

LStringView PlatformMisc::GetTargetConfigPath() noexcept
{
    return LStringView{ PRIVATE_ENGINE_CONFIG_COMPOUND };
}

void PlatformMisc::Private::InvalidateCachedValues()
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
