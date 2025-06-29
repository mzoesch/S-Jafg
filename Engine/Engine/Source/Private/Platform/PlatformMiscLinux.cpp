// Copyright mzoesch. All rights reserved.

#if PLATFORM_LINUX

#include "Platform/PlatformMisc.h"
#include "System/Path.h"
#include "System/Paths.h"

namespace
{

} /* ~Namespace <Anonymous> */

Jafg::LPath Jafg::PlatformMisc::GetEngineRootDirImpl()
{
    LPath RealRootDir { PlatformMisc::GetRealEngineRootDir() };

    while (RealRootDir.IsEmpty() == false)
    {
        if (Paths::DoesFileExist(RealRootDir / "jafg.jafgworkspace"))
        {
            break;
        }

        RealRootDir.PopSubPath();

        continue;
    }

    jassert( RealRootDir.IsEmpty() == false && "Failed to find engine root directory." )

    return RealRootDir;
}

Jafg::LPath Jafg::PlatformMisc::GetRealEngineRootDirImpl()
{
    char Buffer[LAL_PLATFORM_MAX_PATH] = { 0 };
    const i64 Ret = readlink("/proc/self/exe", Buffer, LAL_PLATFORM_MAX_PATH);
    if (Ret == -1)
    {
        panic("Failed to read the symbolic link.");
    }

    Buffer[Ret - 1] = '\0';

    const std::string FromBuffer = Buffer;
    const std::string::size_type Position = FromBuffer.find_last_of('/');
    LPath Path = LPath(FromBuffer.substr(0, Position).c_str());
    Path.Normalize();

    return Path;
}

i32 Jafg::PlatformMisc::GetNumberOfPhysicalViewports()
{
    return 1;
}

bool Jafg::PlatformMisc::SetPhysicalViewports()
{
    // wl_display* D = wl_display_connect(nullptr);
    // if (D == nullptr)
    // {
    //     panic("Failed to connect to wayland. Is the compositor running?");
    //     return false;
    // }
    //
    // wl_registry* R = wl_display_get_registry(D);
    //
    // wl_display_roundtrip(D);
    // while (wl_display_dispatch(D) != -1)
    // {
    // }
    //
    // wl_display_disconnect(D);
    return true;
}

#endif /* PLATFORM_LINUX */
