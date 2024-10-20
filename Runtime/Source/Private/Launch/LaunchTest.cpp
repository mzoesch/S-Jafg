// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"

#if WITH_TESTS

#include "TesterInclude.h"

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
    EPlatformExit::Type ExitCode = EPlatformExit::Success;
    ::Jafg::Tester::RunTests(&ExitCode);

    return ExitCode;
}

#endif /* WITH_TESTS */
