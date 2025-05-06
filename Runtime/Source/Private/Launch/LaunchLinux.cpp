// Copyright mzoesch. All rights reserved.

#if PLATFORM_LINUX

#include "Core/Application.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

using namespace Jafg;

extern EPlatformExit::Type GuardedMain();

namespace
{

bool IsGdb()
{
    char Buffer[4096];

    const i32 Fd = ::open("/proc/self/status", O_RDONLY);
    if (Fd == -1)
    {
        return false;
    }

    const ssize_t num_read = ::read(Fd, Buffer, sizeof(Buffer) - 1);
    ::close(Fd);

    if (num_read <= 0)
    {
        return false;
    }

    Buffer[num_read] = '\0';
    constexpr char PidTracerString[] = "TracerPid:";
    const char* PidTracer = ::strstr(Buffer, PidTracerString);
    if (!PidTracer)
    {
        return false;
    }

    for (const char* characterPtr = PidTracer + sizeof(PidTracerString) - 1; characterPtr <= Buffer + num_read; ++characterPtr)
    {
        if (::isspace(*characterPtr))
        {
            continue;
        }

        return ::isdigit(*characterPtr) != 0 && *characterPtr != '0';
    }

    return false;
}

} /* ~Namespace <Anonymous> */

i32 main(const i32 argc, char *argv[])
{
    i32 ErrorLevel = 0;

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
        LOG_PRIVATE_UNSAFE_FLUSH_EVERYTHING_FAST()
        {
            while (::IsGdb() == false)
            {
                PlatformHal::SleepNoStats(1.0);
                continue;
            }
        }
        LOG_INFO(LogJafgInternal, "Debugger attached - continuing.");
        LOG_PRIVATE_UNSAFE_FLUSH_EVERYTHING_FAST()
    }

    Application::Private::bDebuggerPresent = ::IsGdb();
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

    if (Application::IsPauseBeforeExit())
    {
        LOG_INFO(LogPlatform, "Pausing before exit.")
        LOG_INFO(LogPlatform, "Press any key to continue...")
        LOG_PRIVATE_UNSAFE_FLUSH_EVERYTHING_FAST()
        std::cin.get();
    }

    return ErrorLevel;
}

#endif /* PLATFORM_LINUX */
