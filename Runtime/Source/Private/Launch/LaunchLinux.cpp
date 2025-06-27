// Copyright mzoesch. All rights reserved.

#if PLATFORM_LINUX

#include "Core/Application.h"

using namespace Jafg;

extern EPlatformExit::Type GuardedMain();

i32 main(const i32 argc, char *argv[])
{
    i32 ErrorLevel { 0 };

    LString CmdLine;
    for (i32 i = 1; i < argc; ++i)
    {
        CmdLine += argv[i];
        if (i < argc - 1)
        {
            CmdLine += " ";
        }

        continue;
    }

    Application::Private::CommandLine = std::move(CmdLine);

    if (Application::Private::CommandLine.FindFirst("WaitForDebugger") != INDEX_NONE)
    {
        LOG_INFO(LogJafgInternal, "Waiting for debugger ...");
        LAL_UNSAFE_FLUSH_OUT_STREAMS()
        {
            while (Lal::Hal::IsTracerPidValidVerySlow() == false)
            {
                Lal::Hal::SleepNoStats(1.0);
                continue;
            }
        }
        LOG_INFO(LogJafgInternal, "Debugger attached - continuing.");
        LAL_UNSAFE_FLUSH_OUT_STREAMS()
    }

    Application::Private::bDebuggerPresent = ::Lal::Hal::IsTracerPidValidVerySlow();
    Application::Private::UpdateApplicationCommandLineVariables();

    if (Application::IsDebuggerPresent() && !Application::IsAlwaysReportCrash())
    {
        LOG_INFO(LogPlatform, "Suppressing crash dialog.")
        ErrorLevel = GuardedMain();
    }
    else
    {
        try
        {
            LOG_INFO(LogPlatform, "Leveraging structured exception handling.")
            ErrorLevel = GuardedMain();
        }
        catch (...)
        {
            try
            {
                LAL_UNSAFE_FLUSH_OUT_STREAMS()
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

    if (Application::IsPauseBeforeExit())
    {
        LOG_INFO(LogPlatform, "Pausing before exit.")
        LOG_INFO(LogPlatform, "Press any key to continue...")
        LAL_UNSAFE_FLUSH_OUT_STREAMS()
        std::cin.get();
    }

    return ErrorLevel;
}

#endif /* PLATFORM_LINUX */
