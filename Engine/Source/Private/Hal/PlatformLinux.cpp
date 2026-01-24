// Copyright mzoesch. All rights reserved.

#if PLATFORM_LINUX

#include "Minimal.afx"
#include "Core/Application.h"

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <cstring>
#include <sys/types.h>
#include <execinfo.h>
#include <csignal>
#include <cstdlib>
#include <signal.h>
#include <dirent.h>

void Jafg::LOnPlatformBreakLinux::ExitQuietly()
{
    ::_Exit(EXIT_FAILURE);
}

void Jafg::LOnPlatformBreakLinux::OnProgramPanicImpl
(
    const LPrimitivePlatformTypesGeneric::LJafgChar* InMessage
)
{
    //
    // This does not work properly. We have to stop all threads except the current thread that is running.
    // Then make a memory dump of the process and print the stack trace.
    // Currently, the pthread will still continue which can cause confusion when looking at the memory dump, especially
    // when this panic was not triggered by the master thread.
    //

    const pid_t Pid { ::getpid() };
    const pid_t Tid { ::gettid() };

    ///////////////////////////////////////////////////////////////////////////////
    // pthread kill
    LString TaskPath { "/proc/self/task" };
    if (DIR* Dir { ::opendir(TaskPath.c_str()) }; Dir)
    {
        struct dirent* Entry;

        while ((Entry = ::readdir(Dir)))
        {
            if (Entry->d_type != DT_DIR)
            {
                continue;
            }

            if (const pid_t ThreadTid { atoi(Entry->d_name) }; ThreadTid > 0 && ThreadTid != Tid)
            {
                ::syscall(SYS_tgkill, Pid, ThreadTid, signal);
            }

            continue;
        }

        ::closedir(Dir);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Dump
    void* AddrList[JAFG_PLATFORM_MAX_FRAMES];
    const i32 AddrLen { ::backtrace(AddrList, JAFG_PLATFORM_MAX_FRAMES) };

    const LPath DumpF { Finder::GetMostRecentMemDumpFile() };

    //
    // Just try to dump it. It will obviously not work if the process is being debugged.
    // But we let gcore handle this case.
    //
    std::stringstream Stream;
    Stream << "gdb -p " << Pid
           << " -batch -ex \"gcore " << DumpF.c_str()
           << "\" -ex \"detach\" -ex \"quit\""
           ;
    std::filesystem::create_directories(DumpF.parent_path().c_str());
    LOG_VERBOSE(LogJafgInternal, "Executing memory dump command: [{}].", Stream.str());
    if (const i32 Rc { ::system(Stream.str().c_str()) }; Rc != 0)
    {
        LOG_ERROR(LogPlatform, "Failed to create memory dump for our process [PID: {}]. Return code from [gcore] is [{}].", Pid, Rc);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Error message yea - we like this.
    std::ostringstream TraceStream;
    if (AddrLen == 0)
    {
        TraceStream << "Empty or corrupt.\n";
    }
    else
    {
        char* *const Symbols { ::backtrace_symbols(AddrList, AddrLen) };
        for (i32 i { 0 }; i < AddrLen; ++i)
        {
            TraceStream << Symbols[i] << '\n';
        }
        ::free(Symbols);
    }

    LOG_ERROR(LogJafgInternal, "Fatal Error: [{}].", InMessage)
    LOG_ERROR(LogJafgInternal, "Stacktrace:\n{}", TraceStream.str());

#if WITH_LOCAL_LAYER
    if (Application::Private::bGSuppressCrashDialog == false && Hal::IsTracerPidValidVerySlow() == false)
    {
        std::string ZenityMessage;
        ZenityMessage.reserve(strlen(InMessage));
        for (const char* Ptr { InMessage }; *Ptr; ++Ptr)
        {
            if (*Ptr == '<')
            {
                ZenityMessage += "&lt;";
            }
            else if (*Ptr == '>')
            {
                ZenityMessage += "&gt;";
            }
            else if (*Ptr == '"')
            {
                ZenityMessage += "&quot;";
            }
            else if (*Ptr == '\'')
            {
                ZenityMessage += "&apos;";
            }
            else if (*Ptr == '&')
            {
                ZenityMessage += "&amp;";
            }
            else
            {
                ZenityMessage += *Ptr;
            }
        }

        Jafg::FlushOutStreams();
        const LString Zenity
        {
            Jafg::SprintF
            (
                "zenity --error --title=\"Jafg Panic; We are fucked.\" --text=\"{}\n\nStacktrace:\n{}\" --width=1000",
                ZenityMessage,
                TraceStream.str()
            )
        };
        ::system(Zenity.c_str());
    }
    else
    {
        LOG_VERBOSE(LogJafgInternal, "Suppressed jafg crash dialog window.")
    }
#endif /* WITH_LOCAL_LAYER */

    /* Try writing the dump first, as this is also likely to fail - and then we would not have any dump. */
    const LString TraceContent
    {
        Jafg::SprintF
        (
            "{}\n\nStacktrace:\n{}",
            InMessage,
            TraceStream.str()
        )
    };
    Finder::OverrideFile("Saved/Dumps/stack.trace", TraceContent);

    // Flush, because some streams may be buffered and missing while aborting.
    Jafg::FlushOutStreams();

    ///////////////////////////////////////////////////////////////////////////////
    // The final absolute end.
    if (Hal::IsTracerPidValidVerySlow())
    {
        //# The last resort if the program is being debugged. This is the end.
        JAFG_PLATFORM_BREAK()
    }

    LOnPlatformBreakLinux::ExitQuietly();
}

void Jafg::LOnPlatformBreakLinux::OnProgramPanic
(
    const LPrimitivePlatformTypesGeneric::LJafgChar* InBaseMessage,
    const LPrimitivePlatformTypesGeneric::LJafgChar* InFile,
    const LPrimitivePlatformTypesGeneric::u64        InLine
)
{
    std::ostringstream Stream;
    Stream << "Fuck. Jafg has panicked and lost the war of being a good boy." << "\n\n"
           << InBaseMessage << "\n\n"
           << "~File: " << InFile << "\n"
           << "~Line: " << InLine << "\n\n"
           ;

    LOnPlatformBreakLinux::OnProgramPanicImpl(Stream.str().c_str());
}

namespace Jafg::Hal
{

void SleepNoStats(const f64 InSeconds)
{
    if (const i32 Micro{ static_cast<i32>(InSeconds * maths::s2mus_d) }; Micro > 0)
    {
        usleep(Micro);
    }
    else
    {
        YieldThread();
    }

    return;
}

void YieldThread()
{
    sched_yield();
    return;
}

//
// Note that this function only currently works with GDB.
//
bool IsTracerPidValidVerySlow()
{
    char Buffer[4096];

    const i32 Fd { ::open("/proc/self/status", O_RDONLY) };
    if (Fd == -1)
    {
        return false;
    }

    const ssize_t NumRead { ::read(Fd, Buffer, sizeof(Buffer) - 1) };
    ::close(Fd);

    if (NumRead <= 0)
    {
        return false;
    }

    Buffer[NumRead] = '\0';
    constexpr char PidTracerString[] { "TracerPid:" };
    const char* PidTracer { ::strstr(Buffer, PidTracerString) };
    if (!PidTracer)
    {
        return false;
    }

    for (const char* CharacterPtr { PidTracer + sizeof(PidTracerString) - 1 }; CharacterPtr <= Buffer + NumRead; ++CharacterPtr)
    {
        if (::isspace(*CharacterPtr))
        {
            continue;
        }

        return ::isdigit(*CharacterPtr) != 0 && *CharacterPtr != '0';
    }

    return false;
}

} /* ~Namespace Jafg::Hal */

#endif /* PLATFORM_LINUX */
