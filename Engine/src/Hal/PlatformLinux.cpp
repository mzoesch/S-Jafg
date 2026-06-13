// Copyright mzoesch. All rights reserved.

#if JAFG_PLATFORM_LINUX

#include "Minimal.afx"
#include "Core/App.h"

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

void Jafg::App::Detail::TrapMeFn::operator()() const noexcept
{
    ::_Exit(EXIT_FAILURE);
}

void Jafg::App::Detail::TrapMeFn::operator()(std::string_view Message) const noexcept
{
    const pid_t Pid{::getpid()};
    // const pid_t Tid{::gettid()};

    if (Finder::Detail::_gdb)
    {
        std::stringstream Stream;
        Stream << *Finder::Detail::_gdb << " -p " << Pid
               << " -batch -ex \"gcore " << Finder::Detail::DumpFile
               << "\" -ex \"detach\" -ex \"quit\""
               ;
        LOG_VERBOSE(LogJafgInternal, "Executing memory dump command: [{}].", Stream.str());

        if (auto Rc{std::system(Stream.str().c_str())}; Rc != 0)
        {
            LOG_ERROR(LogPlatform, "Failed to create memory dump for our process [PID: {}]. Return code from [gdb] is [{}].", Pid, Rc);
        }
    }
    else
    {
        // Immediate raise. Let the OS handle the dump and crash report if possible.
        ::raise(SIGABRT);
        (*this)();
    }

    LOG_ERROR(LogJafgInternal, "Fatal Error: [{}].", Message)

    if (bDumpStack)
    {
        std::stringstream Stream;
        Stream << *Finder::Detail::_gdb << " -batch -ex \"thread apply all bt full\" "
               << GetExpectedRuntimePath() << " " << Finder::Detail::DumpFile << " > "
               << absolute(Finder::GetMostRecentStackTraceFile());
        LOG_VERBOSE(LogJafgInternal, "Executing stack trace dump command: [{}].", Stream.str());
        if (auto Rc{std::system(Stream.str().c_str())}; Rc != 0)
        {
            LOG_ERROR(LogPlatform, "Failed to create stack trace dump for our process [PID: {}]. Return code from [gdb] is [{}].", Pid, Rc);
        }
    }

#if JAFG_WITH_LOCAL_LAYER
    if (!App::Detail::bSuppressCrashDialog && !IsTracerPidValidVerySlow())
    {
        auto EscapeMessage{[](LStringView String)
        {
            LString Result; Result.reserve(String.size()*2);
            for (auto Char : String)
            {
                if (Char == '<')
                {
                    Result += "&lt;";
                }
                else if (Char == '>')
                {
                    Result += "&gt;";
                }
                else if (Char == '"')
                {
                    Result += "&quot;";
                }
                else if (Char == '\'')
                {
                    Result += "&apos;";
                }
                else if (Char == '&')
                {
                    Result += "&amp;";
                }
                else
                {
                    Result += Char;
                }
            }
            return Result;
        }};

        void* AddrList[JAFG_PLATFORM_MAX_FRAMES];
        std::ostringstream Stream;
        if (i32 AddrLen{::backtrace(AddrList, JAFG_PLATFORM_MAX_FRAMES)}; AddrLen <= 0)
        {
            Stream << "Empty or corrupt.\n";
        }
        else
        {
            char** const Symbols{::backtrace_symbols(AddrList, AddrLen)};
            for (auto Idx{0uz}; Idx < static_cast<std::size_t>(AddrLen); ++Idx)
            {
                Stream << Symbols[Idx] << '\n';
            }
            ::free(Symbols);
        }

        Jafg::FlushOutStreams();
        (void)std::system(algo::sprintf(
            "zenity --error --title=\"Jafg Panic; We are fucked.\" --text=\"{}\n\nStacktrace:\n{}\" --width=1920",
            EscapeMessage(Message), EscapeMessage(Stream.str())).c_str());
    }
    else
    {
        LOG_VERBOSE(LogJafgInternal, "Suppressed jafg crash dialog window.")
    }
#endif /* JAFG_WITH_LOCAL_LAYER */

    ///////////////////////////////////////////////////////////////////////////////
    // The final absolute end.
    if (IsTracerPidValidVerySlow())
    {
        //# The last resort if the program is being debugged. This is the end.
        JAFG_PLATFORM_BREAK()
    }
    (*this)();
}

void Jafg::App::SleepNoStats(f64 InSeconds)
{
    if (i32 Micro{static_cast<i32>(InSeconds * maths::s2mus_d)}; Micro > 0)
    {
        ::usleep(Micro);
    }
    else
    {
        std::this_thread::yield();
    }

    return;
}

//
// Note that this function only currently works with GDB.
//
bool Jafg::App::Detail::IsTracerPidValidVerySlow()
{
    char Buffer[4096];

    auto Fd{::open("/proc/self/status", O_RDONLY)};
    if (Fd == -1)
    {
        return false;
    }

    auto NumRead{::read(Fd, Buffer, sizeof(Buffer) - 1)};
    ::close(Fd);

    if (NumRead <= 0)
    {
        return false;
    }

    Buffer[NumRead] = '\0';
    constexpr char PidTracerString[]{"TracerPid:"};
    char const* PidTracer{::strstr(Buffer, PidTracerString)};
    if (!PidTracer)
    {
        return false;
    }

    for (char const* CharacterPtr{PidTracer + sizeof(PidTracerString) - 1}; CharacterPtr <= Buffer + NumRead; ++CharacterPtr)
    {
        if (::isspace(*CharacterPtr))
        {
            continue;
        }
        return ::isdigit(*CharacterPtr) != 0 && *CharacterPtr != '0';
    }

    return false;
}

#endif /* JAFG_PLATFORM_LINUX */
