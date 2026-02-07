// Copyright mzoesch. All rights reserved.

#if PLATFORM_LINUX

#include "Core/Application.h"
#include "Platform/PlatformMisc.h"
#include <csignal>

using namespace Jafg;

extern EPlatformExit::Type GuardedMain();

namespace
{

NORETURN
void SignumPosixAction_JafgHandler_Fatal(const i32 InSignal, siginfo_t* InInfo, void* InContext)
{
    if (Application::Private::bGAlreadyCrashed)
    {
        LOG_ERROR(LogJafgInternal, "Already crashed - ignoring signal [{}].", InSignal)
        LOnPlatformBreak::ExitQuietly();
    }

    Application::Private::bGAlreadyCrashed = true;

    u64 Cursor { 0 };
    char Emitted[37 + 7 + 30 /* 30 padding */];

    ::strcpy(Emitted, "Low Level Fatal Error: Posix Signum [");
    Cursor += 37;

    if (const char* Name { ::strsignal(InSignal) }; Name)
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
    if (InInfo && InInfo->si_addr)
    {
        char AddrStr[20];
        ::snprintf(AddrStr, sizeof(AddrStr), "%p", InInfo->si_addr);
        ::write(STDERR_FILENO, AddrStr, ::strlen(AddrStr));
    }
    else
    {
        ::write(STDERR_FILENO, "unknown", 7);
    }
    ::write(STDERR_FILENO, "].\n", 3);

    Jafg::LOnPlatformBreak::OnProgramPanicImpl(Emitted);
}

NORETURN
void SignumPosixAction_JafgHandler_NotSoFatal(const i32 InSignal, siginfo_t* InInfo, void* InContext)
{
    //
    // Just do anything normally, but do not show the annoying crash report dialog window.
    //
    Application::Private::bGSuppressCrashDialog = true;

    SignumPosixAction_JafgHandler_Fatal(InSignal, InInfo, InContext);
}

} /* ~Namespace <Anonymous> */

i32 main(const i32 ArgC, const char* ArgV[])
{
    i32 ErrorLevel{};

    TArray<LString> Arguments;
    for (i32 Idx{ 1 }; Idx < ArgC; ++Idx)
    {
        Arguments.emplace_back(ArgV[Idx]);
    }
    Application::Private::RawCommandLine = std::move(Arguments);

    if (algo::contains(Application::GetRawCmdLine(), "-WaitForDebugger"))
    {
        Application::Private::WaitForDebuggerGracefully(true);
    }

    //
    // We use '-fno-exceptions' therefore we cannot use C++ exceptions and have to deal
    // with process signals manually by taking directly to the POSIX API. Omg I want to ***.
    //
    // @see https://man7.org/linux/man-pages/man2/sigaction.2.html
    //
    struct sigaction Action_Fatal {}; // Pretty bad.
    struct sigaction Action_Error {}; // Not so fatal.

    // "On some architectures a union is involved: do not assign to both sa_handler and sa_sigaction." <-- Lol, idiots
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

    ///////////////////////////////////////////////////////////////////////////////
    // ISO C99
    ::sigaction(SIGABRT, &Action_Fatal, nullptr); /* Abnormal termination from ::abort. */
    ::sigaction(SIGFPE,  &Action_Fatal, nullptr); /* Erroneous arithmetic operation. */
    ::sigaction(SIGHUP,  &Action_Error, nullptr); /* Hangup signal of terminal or helicopter parent. */ // We do not really care.
    ::sigaction(SIGILL,  &Action_Fatal, nullptr); /* Illegal instruction. */
    ::sigaction(SIGINT,  &Action_Error, nullptr); /* Interactive attention signal. */ // -> Non-fatal because the user is at fault then (because he cannot use his keyboard), not us.
    ::sigaction(SIGSEGV, &Action_Fatal, nullptr); /* Invalid memory reference */ // <- The usual suspect
    ::sigaction(SIGTERM, &Action_Error, nullptr); /* Termination signal. */ // The default behavior for SIGTERM is to terminate the process, but we want to handle it gracefully.

    ///////////////////////////////////////////////////////////////////////////////
    // Historical POSIX.
    ::sigaction(SIGQUIT, &Action_Fatal, nullptr);
    ::sigaction(SIGTRAP, &Action_Fatal, nullptr); /* Trace or breakpoint trap. */
    // ::sigaction(SIGKILL, &Action_Fatal, nullptr); /* Trace or breakpoint trap. */ // Cannot be used as a signum - sadly.

    ///////////////////////////////////////////////////////////////////////////////
    // Historical POSIX (Arch).
    ::sigaction(SIGBUS,  &Action_Fatal, nullptr); /* Bus error (bad memory access). */
    ::sigaction(SIGSYS,  &Action_Fatal, nullptr); /* Bad system call (SVr4). */

    ErrorLevel = GuardedMain();

    if (Application::IsPauseBeforeExit())
    {
        LOG_INFO(LogPlatform, "Pausing before exit.")
        LOG_INFO(LogPlatform, "Press any key to continue ...")
        Jafg::FlushOutStreams();

        std::cin.get();
    }

    return ErrorLevel;
}

#endif /* PLATFORM_LINUX */
