// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"

#if !PLATFORM_WINDOWS
    #error "Is not PLATFORM_WINDOWS but platform specific code is being compiled."
#endif /* !PLATFORM_WINDOWS */

#include <iostream>
#include <Windows.h>

extern EPlatformExit::Type GuardedMain(const TChar* CmdLine);

bool GPauseBeforeExit = false;

int32 LaunchWin(HINSTANCE hInInstance, HINSTANCE hPrevInstance, char*, int32 nCmdShow, const TCHAR* CmdLine)
{
    int32 ErrorLevel = 0;

    if (CmdLine == nullptr)
    {
        CmdLine = ::GetCommandLineW();
    }

    ErrorLevel = GuardedMain(PlatformTypes::Ws2CStr(CmdLine));

    return ErrorLevel;
}

void LaunchWinShutdown()
{
    if (GPauseBeforeExit)
    {
        std::cout << "Press any key to continue..." << '\n';
        std::cin.get();
    }

    return;
}

int32 WINAPI WinMain(_In_ HINSTANCE hInInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ char* pCmdLine, _In_ int32 nCmdShow)
{
    /*
     * If LNK2019 [int __cdecl __scrt_common_main_seh(void)] make sure to set the System-Linker of the Runtime
     * Project to use the subsystem "Not Set" (for automatic platform detection) or "Windows".
     */
    const int32 Result = LaunchWin(hInInstance, hPrevInstance, pCmdLine, nCmdShow, nullptr);
    LaunchWinShutdown();
    return Result;
}
