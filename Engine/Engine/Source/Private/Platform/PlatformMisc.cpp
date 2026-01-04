// Copyright mzoesch. All rights reserved.

#include "Platform/PlatformMisc.h"
#include "Async/TaskUtility.h"

namespace Jafg
{

LPath PlatformMisc::GetEngineRootDir()
{
#if WITH_VIRTUAL_FILESYSTEM
    LOG_WARNING(LogSystem, "Access to the filesystem is denied on this platform. Tried to access: [ENGINE_ROOT_DIR].")
    return "";
#else /* WITH_VIRTUAL_FILESYSTEM */
    static LPath CachedEngineRootDir;
    if (CachedEngineRootDir.empty())
    {
        check( Tasks::IsOnMasterThread() )

        LPath RealRootDir{ PlatformMisc::GetSelfProcDir() };
        while (RealRootDir.empty() == false)
        {
            if (Finder::DoesFileExist(RealRootDir / "jafg.jafgworkspace"))
            {
                break;
            }
            RealRootDir.assign(RealRootDir.parent_path());
            continue;
        }
        CachedEngineRootDir = RealRootDir;
    }

    check( Finder::DoesFileExist(CachedEngineRootDir / "jafg.jafgworkspace") )
    return CachedEngineRootDir;
#endif /* !WITH_VIRTUAL_FILESYSTEM */
}

LPath PlatformMisc::GetSelfProcDir()
{
#if WITH_VIRTUAL_FILESYSTEM
    LOG_WARNING(LogSystem, "Access to the filesystem is denied on this platform. Tried to access: [SELF_PROC_DIR].")
    return "";
#else /* WITH_VIRTUAL_FILESYSTEM */
    static LPath CachedSelfProcDir;
    if (CachedSelfProcDir.empty())
    {
        check( Tasks::IsOnMasterThread() )

#if PLATFORM_LINUX
        char Buffer[LAL_PLATFORM_MAX_PATH] = { 0 };
        auto Ret{ readlink("/proc/self/exe", Buffer, LAL_PLATFORM_MAX_PATH) };
        if (Ret == -1)
        {
            panic("Failed to read the symbolic link.")
        }
        Buffer[Ret] = '\0';
        CachedSelfProcDir = LString{ Buffer };
        CachedSelfProcDir = CachedSelfProcDir.parent_path();
#elif PLATFORM_WINDOWS
    #error "Missing implementation for this platform."
#else /* PLATFORM_WINDOWS */
    #error "Missing implementation for this platform."
#endif /* !PLATFORM_WINDOWS */
    }

    return CachedSelfProcDir;
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

} /* ~Namespace Jafg */
