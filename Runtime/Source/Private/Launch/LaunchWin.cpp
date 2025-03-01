// Copyright mzoesch. All rights reserved.

#if PLATFORM_WINDOWS

#include "Core/Application.h"

#if WITH_TESTS
    extern EPlatformExit::Type TestAnsiMain(char* CmdLine);
    extern EPlatformExit::Type TestWideMain(wchar_t* CmdLine);

    #ifdef UNICODE
        #define TestMain        TestWideMain
    #else /* UNICODE */
        #define TestMain        TestAnsiMain
    #endif /* !UNICODE */

#endif /* WITH_TESTS */

using namespace Jafg;

extern EPlatformExit::Type GuardedMain();

namespace
{

bool GPauseBeforeExit = false;

} /* ~Namespace <Anonymous> */

int32 WINAPI WinMain(_In_ HINSTANCE hInInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ char* pCmdLine, _In_ int32 nCmdShow)
{
    //
    // If LNK2019 [int __cdecl __scrt_common_main_seh(void)] make sure to set the System-Linker of the Runtime
    // Project to use the subsystem "Not Set" (for automatic platform detection) or "Windows".
    //
#if WITH_TESTS
    TestMain(::GetCommandLine());
#else /* WITH_TESTS */

    int32 ErrorLevel = 0;

    LString CmdLine;
    if (const wchar_t* CmdLineW = GetCommandLineW(); CmdLineW)
    {
        CmdLine = LPlatformTypes::Ws2S(CmdLineW).c_str();
    }

    Application::Private::CommandLine      = std::move(CmdLine);
    Application::Private::bDebuggerPresent = static_cast<bool>(::IsDebuggerPresent());
    Application::Private::UpdateApplicationCommandLineVariables();
    GPauseBeforeExit = Application::HasCmdLineParameter("PauseBeforeExit");

    if (Application::IsDebuggerPresent() && !Application::IsAlwaysReportCrash())
    {
        LOG_INFO(LogPlatform, "Suppressing crash dialog.")
        ErrorLevel = GuardedMain();
    }
    else
    {
        try // Mmm, this only works for C++ exceptions. We should implement a custom exception handler with WIN-SEH.
        {
            LOG_INFO(LogPlatform, "Leveraging structured exception handling.")
            ErrorLevel = GuardedMain();
        }
        catch (...)
        {
            try
            {
                LOG_PRIVATE_UNSAFE_FLUSH_EVERYTHING_FAST()
                throw;
            }
            catch (const std::exception& E)
            {
                LOG_ERROR(LogPlatform, "Guarded main failed with: {}.", E.what())
            }
            catch (const char* Msg)
            {
                LOG_ERROR(LogPlatform, "Guarded main failed with: {}.", Msg)
            }
            catch (int Num)
            {
                LOG_ERROR(LogPlatform, "Guarded main failed with: {}.", Num)
            }
            catch (...)
            {
                LOG_ERROR(LogPlatform, "Guarded main failed with an unknown exception.")
            }
        }
    }

    LOG_PRIVATE_UNSAFE_FLUSH_EVERYTHING_FAST()

    if (GPauseBeforeExit)
    {
        LOG_INFO(LogPlatform, "Pausing before exit.")
        LOG_INFO(LogPlatform, "Press any key to continue...")
        LOG_PRIVATE_UNSAFE_FLUSH_EVERYTHING_FAST()
        std::cin.get();
    }

    return ErrorLevel;

#endif /* !WITH_TESTS */
}

#ifdef TestMain
    #undef TestMain
#endif /* TestMain */

#endif /* PLATFORM_WINDOWS */
