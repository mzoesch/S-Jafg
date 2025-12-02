// Copyright mzoesch. All rights reserved.

#if PLATFORM_DESKTOP

#include "Platform/PlatformMisc.h"

LPath Jafg::PlatformMisc::Private::GetEngineRootDirImpl()
{
    LPath RealRootDir{ PlatformMisc::GetRealEngineRootDir() };

    while (RealRootDir.empty() == false)
    {
        if (Finder::DoesFileExist(RealRootDir / "jafg.jafgworkspace"))
        {
            break;
        }

        RealRootDir.assign(RealRootDir.parent_path());

        continue;
    }

    jassert( RealRootDir.empty() == false && "Failed to find engine root directory." )

    return RealRootDir;
}


#endif /* PLATFORM_DESKTOP */
