// Copyright mzoesch. All rights reserved.

#if PLATFORM_WINDOWS

#include "AbsoluteMinimalCore.h"

void Lal::LOnPlatformBreakWindows::ExitQuietly()
{
    ::_Exit(EXIT_FAILURE);
}

void Lal::LOnPlatformBreakWindows::OnProgramPanicImpl
(
    LPrimitivePlatformTypesGeneric::LJafgChar const* InMessage
)
{
    // TODO: Stop threads.

#if WITH_LOCAL_LAYER
    if (JafgCore::bGSuppressCrashDialog == false && Lal::Hal::IsTracerPidValidVerySlow() == false)
    {
        const LWString Caption{ LITERAL_WIDE("Jafg panicked; We are fucked.") };
        const auto Message{ Lal::Utf8ToUtf16(InMessage) };

        MessageBox(
            nullptr,
            Message.c_str(),
            Caption.c_str(),
            MB_ICONERROR | MB_OK
            );
    }
    else
    {
        LOG_VERBOSE(LogJafgInternal, "Suppressed jafg crash dialog window.")
    }
#endif /* WITH_LOCAL_LAYER */

    // TODO: Stacktrace

    // Flush, because some streams may be buffered and missing while aborting.
    Lal::FlushOutStreams();

    ///////////////////////////////////////////////////////////////////////////////
    // The final absolute end.
    if (Hal::IsTracerPidValidVerySlow())
    {
        //# The last resort if the program is being debugged. This is the end.
        LAL_PLATFORM_BREAK()
    }

    LOnPlatformBreakWindows::ExitQuietly();
}

void Lal::LOnPlatformBreakWindows::OnProgramPanic
(
    LPrimitivePlatformTypesGeneric::LJafgChar const* InBaseMessage,
    LPrimitivePlatformTypesGeneric::LJafgChar const* InFile,
    LPrimitivePlatformTypesGeneric::u64       const  InLine
)
{
    std::ostringstream Stream;
    Stream << "Fuck. Jafg entered an one-way enclosing block inside a critical control path and lost the war of being a good boy." << "\n\n"
           << InBaseMessage << "\n\n"
           << "~File: " << InFile << "\n"
           << "~Line: " << InLine
           ;

    LOnPlatformBreakWindows::OnProgramPanicImpl(Stream.str().c_str());
}

namespace Lal::Hal
{

void SleepNoStats(const double InSeconds)
{
    if (const DWORD Milli{ static_cast<DWORD>(InSeconds * LAL_S2MS_D) }; Milli > 0)
    {
        ::Sleep(Milli);
    }
    else
    {
        YieldThread();
    }

    return;
}

void YieldThread()
{
    ::SwitchToThread();
    return;
}

bool IsTracerPidValidVerySlow()
{
    return ::IsDebuggerPresent();
}

} /* ~Namespace Lal::Hal */

#endif /* PLATFORM_WINDOWS */
