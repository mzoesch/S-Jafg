// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"

#if WITH_TESTS

#include "TesterInclude.h"

EPlatformExit::Type LalLaunchTestMain(void);

EPlatformExit::Type LalTestAnsiMain(i32 Argc, char* Argv[])
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

    std::cout.flush();

    return ExitCode;
}

#if PLATFORM_WINDOWS
    i32 WINAPI WinMain(_In_ HINSTANCE hInInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ char* pCmdLine, _In_ i32 nCmdShow)
#else /* PLATFORM_WASM */
    i32 main(i32 Argc, char* Argv[])
#endif /* !PLATFORM_WASM */
{
#if WITH_TESTS
    #if PLATFORM_WINDOWS
        return LalTestWideMain(::GetCommandLineW());
    #else
        return LalTestAnsiMain(Argc, Argv);
    #endif
#else /* WITH_TESTS */
    return EPlatformExit::Success;
#endif /* !WITH_TESTS */
}

#endif /* WITH_TESTS */
