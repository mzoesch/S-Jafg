// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"

#if WITH_TESTS

EPlatformExit::Type LaunchTestMain(void);

EPlatformExit::Type TestAnsiMain(char* CmdLine)
{
    return LaunchTestMain();
}

EPlatformExit::Type TestWideMain(wchar_t* CmdLine)
{
    return LaunchTestMain();
}

EPlatformExit::Type LaunchTestMain(void)
{
    std::cout << "Test main." << '\n';

    return EPlatformExit::Success;
}

#endif /* WITH_TESTS */
