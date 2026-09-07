// Copyright mzoesch. All rights reserved.

#if JAFG_PLATFORM_LINUX

#include "Core/App.h"
#include "Engine/Engine.h"
#include <csignal>
#include <sys/resource.h>
#include <sys/prctl.h>

#if JAFG_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
    #pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
    #pragma clang diagnostic ignored "-Wunsafe-buffer-usage-in-libc-call"
#endif /* JAFG_WITH_CLANG */

extern EPlatformExit::Type AgnosticLaunch();

using namespace Jafg;

namespace
{

NORETURN
void SignumPosixAction_JafgHandler_Fatal(i32 Signal, siginfo_t* Info, void* InContext)
{
    (void)InContext;

    if (App::Detail::bAlreadyCrashed)
    {
        LOG_ERROR(LogJafgInternal, "Already crashed - ignoring signal [{}].", Signal)
        App::Detail::TrapMe();
    }

    App::Detail::bAlreadyCrashed = true;

    u64 Cursor{};
    char Emitted[37 + 7 + 30 /* 30 padding */];

    ::strcpy(Emitted, "Low Level Fatal Error: Posix Signum [");
    Cursor += 37;

    if (const char* Name{::strsignal(Signal)}; Name)
    {
        ::strcpy(Emitted + Cursor, Name);
        Cursor += ::strlen(Name);
    }
    else
    {
        ::strcpy(Emitted + Cursor, "unknown");
        Cursor += 7;
    }
    ::strcpy(Emitted + Cursor, "].");
    Cursor += 2;
    ::write(STDERR_FILENO, Emitted, Cursor);
    ::write(STDERR_FILENO, "\n", 1);

    ::write(STDERR_FILENO, "Address: [", 10);
    if (Info && Info->si_addr)
    {
        char AddrStr[20];
        ::snprintf(AddrStr, sizeof(AddrStr), "%p", Info->si_addr);
        ::write(STDERR_FILENO, AddrStr, ::strlen(AddrStr));
    }
    else
    {
        ::write(STDERR_FILENO, "unknown", 7);
    }
    ::write(STDERR_FILENO, "].\n", 3);

    App::Detail::TrapMe(Emitted);
}

NORETURN
void SignumPosixAction_JafgHandler_NotSoFatal(i32 Signal, siginfo_t* Info, void* Context)
{
    /* Just do anything normally, but do not show the annoying crash report dialog window. */
    App::Detail::bSuppressCrashDialog = true;
    SignumPosixAction_JafgHandler_Fatal(Signal, Info, Context);
}

void SignumPosixAction_JafgHandler_Exit(i32 InSignal, siginfo_t*, void*)
{
    LString Signal{"<unknown>"};
    if (const char* Name{::strsignal(InSignal)}; Name)
    {
        Signal = Name;
    }
    App::RequestEngineExit(algo::sprintf("Received signal [{}]: {}", InSignal, Signal));
}

} /* ~Namespace <Anonymous> */

i32 main(i32 c, char const* v[])
{
    i32 ErrorLevel{};

    check(c > 0)
    TArray<LString> Arguments; algo::for_each(v + 1, v + c, [&Arguments](auto* Arg){ Arguments.emplace_back(Arg); });
    App::Detail::RawCommandLine = std::move(Arguments);

    if (App::Detail::HasArgumentToWaitForDebuggerVeryEarlyOnly())
    {
        App::Detail::WaitForDebuggerGracefully(true);
    }

    //
    // We use '-fno-exceptions' therefore we cannot use C++ exceptions and have to deal
    // with process signals manually by taking directly to the POSIX API.
    //
    // @see https://man7.org/linux/man-pages/man2/sigaction.2.html
    //
    struct sigaction Action_Fatal {}; // Pretty bad.
    struct sigaction Action_Error {}; // Not so fatal.
    struct sigaction Action_Exit  {}; // Exit.

    // "On some architectures a union is involved: do not assign to both sa_handler and sa_sigaction."
    Action_Fatal.sa_sigaction = SignumPosixAction_JafgHandler_Fatal;
    Action_Fatal.sa_flags =

        // "If SA_SIGINFO is specified in sa_flags, then sa_sigaction (instead of sa_handler)
        // specifies the signal-handling function for signum. [...]"
          SA_SIGINFO

        // "Call the signal handler on an alternate signal stack provided by sigaltstack.
        // If an alternate stack is not available, the default stack will be used.
        // This flag is meaningful only when establishing a signal handler."
        | SA_ONSTACK // I guess useful when hitting infinite recursion ... but who programs this bad - Vibe Coders, maybe?
        ;

    Action_Error.sa_sigaction = SignumPosixAction_JafgHandler_NotSoFatal;
    Action_Error.sa_flags = SA_SIGINFO;

    Action_Exit.sa_sigaction = SignumPosixAction_JafgHandler_Exit;
    Action_Exit.sa_flags = SA_SIGINFO;

    ///////////////////////////////////////////////////////////////////////////////
    // ISO C99
    ::sigaction(SIGABRT, &Action_Fatal, nullptr); /* Abnormal termination from ::abort. */
    ::sigaction(SIGFPE,  &Action_Fatal, nullptr); /* Erroneous arithmetic operation. */
    ::sigaction(SIGHUP,  &Action_Error, nullptr); /* Hangup signal of terminal or helicopter parent. */ // We do not really care.
    ::sigaction(SIGILL,  &Action_Fatal, nullptr); /* Illegal instruction. */
    ::sigaction(SIGINT,  &Action_Exit,  nullptr); /* Interactive attention signal. */
    ::sigaction(SIGSEGV, &Action_Fatal, nullptr); /* Invalid memory reference */ // <- The usual suspect
    ::sigaction(SIGTERM, &Action_Exit,  nullptr); /* Termination signal. */ // The default behavior for SIGTERM is to terminate the process, but we want to handle it gracefully.

    ///////////////////////////////////////////////////////////////////////////////
    // Historical POSIX.
    ::sigaction(SIGQUIT, &Action_Fatal, nullptr);
    ::sigaction(SIGTRAP, &Action_Fatal, nullptr); /* Trace or breakpoint trap. */
    // ::sigaction(SIGKILL, &Action_Fatal, nullptr); /* Trace or breakpoint trap. */ // Cannot be used as a signum - sadly.

    ///////////////////////////////////////////////////////////////////////////////
    // Historical POSIX (Arch).
    ::sigaction(SIGBUS,  &Action_Fatal, nullptr); /* Bus error (bad memory access). */
    ::sigaction(SIGSYS,  &Action_Fatal, nullptr); /* Bad system call (SVr4). */

    {
        /* TODO: Is this safe? Probably not -- fix */
        struct rlimit rl{RLIM_INFINITY, RLIM_INFINITY};
        setrlimit(RLIMIT_CORE, &rl);
        prctl(PR_SET_DUMPABLE, 1);
    }

    if (char const* EnvPtr{std::getenv("PATH")})
    {
        LString Env{EnvPtr};

        auto Start{0uz};
        while (Start < Env.size())
        {
            auto End{Env.find(':', Start)};
            if (End == std::string::npos)
            {
                End = Env.size();
            }
            if (auto Candidate{algo::sub<LString, LPath>(Env, Start, End) / "gdb"};
                std::filesystem::exists(Candidate) && std::filesystem::is_regular_file(Candidate))
            {
                auto Perms{std::filesystem::status(Candidate).permissions()};
                if (   (Perms & std::filesystem::perms::owner_exec ) != std::filesystem::perms::none
                    || (Perms & std::filesystem::perms::group_exec ) != std::filesystem::perms::none
                    || (Perms & std::filesystem::perms::others_exec) != std::filesystem::perms::none)
                {
                    finder::detail::_lnx_gdb = Candidate;
                    break;
                }
            }
            Start = End + 1;
        }
    }
    LOG_VERBOSE(LogInformation, "gdb={}", finder::detail::_lnx_gdb ? *finder::detail::_lnx_gdb : "<not found>")

    ErrorLevel = AgnosticLaunch();

    if (App::IsPauseBeforeExit())
    {
        LOG_INFO(LogPlatform, "Pausing before exit.")
        LOG_INFO(LogPlatform, "Press any key to continue...")
        Detail::EmitAndFlushLogs();

        std::cin.get();
    }

    return ErrorLevel;
}

#if JAFG_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* JAFG_WITH_CLANG */

#endif /* JAFG_PLATFORM_LINUX */
