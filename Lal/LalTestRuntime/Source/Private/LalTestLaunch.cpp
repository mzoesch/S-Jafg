// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"

#if WITH_TESTS

#include "TesterInclude.h"

EPlatformExit::Type LalLaunchTestMain(void);

EPlatformExit::Type LalTestAnsiMain(char* CmdLine)
{
    return LalLaunchTestMain();
}

EPlatformExit::Type LalTestWideMain(wchar_t* CmdLine)
{
    return LalLaunchTestMain();
}

EPlatformExit::Type LalLaunchTestMain(void)
{
    EPlatformExit::Type ExitCode = EPlatformExit::Success;
    ::Jafg::Tester::RunTests(&ExitCode);

    return ExitCode;
}

#endif /* WITH_TESTS */

int32 WINAPI WinMain(_In_ HINSTANCE hInInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ char* pCmdLine, _In_ int32 nCmdShow)
{
#if WITH_TESTS
    return LalTestWideMain(::GetCommandLineW());
#else /* WITH_TESTS */
    return EPlatformExit::Success;
#endif /* !WITH_TESTS */
}