// Copyright mzoesch. All rights reserved.

#if PLATFORM_WINDOWS

#include "Minimal.afx"
#include "Core/Application.h"

#include <dbghelp.h>

void Jafg::LOnPlatformBreakWindows::ExitQuietly()
{
    ::_Exit(EXIT_FAILURE);
}

void Jafg::LOnPlatformBreakWindows::OnProgramPanicImpl
(
    LPrimitivePlatformTypesGeneric::LJafgChar const* InMessage
)
{
    const auto Proc{::GetCurrentProcess()};
    const auto Pid{::GetCurrentProcessId()};
    const auto Tid{::GetCurrentThreadId()};

    // TODO: Stop threads.

    ///////////////////////////////////////////////////////////////////////////////
    // Dump
    const LPath DumpF{Finder::GetMostRecentMemDumpFile()};
    auto hFile{::CreateFileW(
          DumpF.c_str()
        , GENERIC_WRITE
        , 0
        , NULL
        , CREATE_ALWAYS
        , FILE_ATTRIBUTE_NORMAL
        , NULL
        )};
    if (hFile != INVALID_HANDLE_VALUE)
    {
        if (MiniDumpWriteDump(Proc, Pid, hFile,
            static_cast<MINIDUMP_TYPE>(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory), NULL, NULL, NULL)
            == FALSE)
        {
            LOG_ERROR(LogPlatform,
                "Failed to create memory dump for our process [PID: [{}] with TID: [{}]]. Platform error: [{}].",
                Pid, Tid, ::GetLastError()
                )
        }
        else
        {
            LOG_VERBOSE(LogPlatform,
                "Memory dump created at [{}] for our process [PID: [{}] with TID: [{}]].",
                DumpF, Pid, Tid
                )
        }
        ::CloseHandle(hFile);
    }
    else
    {
        LOG_ERROR(LogPlatform, "Failed to create or open memory dump file at [{}] for our process [PID: [{}] with TID: [{}]]. Platform error: [{}].",
            DumpF, Pid, Tid, ::GetLastError()
            )
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Backtrace
    void* Stack[JAFG_PLATFORM_MAX_FRAMES];
    USHORT Frames{::CaptureStackBackTrace(0, JAFG_PLATFORM_MAX_FRAMES, Stack, NULL)};

    ///////////////////////////////////////////////////////////////////////////////
    // Stdout
    LOG_ERROR(LogJafgInternal, "Fatal Error: [{}].", InMessage)
    if (SymInitialize(GetCurrentProcess(), NULL, TRUE))
    {
        SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(
            sizeof(SYMBOL_INFO) + 256 * sizeof(char),
            1
            );
        symbol->MaxNameLen = 255;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

        IMAGEHLP_LINE64 line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

        DWORD displacement;

        for (USHORT i = 0; i < Frames; ++i)
        {
            DWORD64 address = (DWORD64)(Stack[i]);

            if (SymFromAddr(Proc, address, 0, symbol))
            {
                if (SymGetLineFromAddr64(Proc, address, &displacement, &line))
                {
                    printf(
                        "#%02u %s (%s:%lu)\n",
                        i,
                        symbol->Name,
                        line.FileName,
                        line.LineNumber
                    );
                }
                else
                {
                    printf(
                        "#%02u %s (no line info)\n",
                        i,
                        symbol->Name
                    );
                }
            }
            else
            {
                printf("#%02u [unknown]\n", i);
            }
        }

        free(symbol);
        SymCleanup(Proc);
    }
    else
    {
        LOG_ERROR(LogJafgInternal, "Failed to retrieve human readable backtrace.")
        std::ostringstream TraceStream;
        for (USHORT Idx{0uz}; Idx < Frames; ++Idx)
        {
            TraceStream << Stack[Idx] << '\n';
        }
        LOG_ERROR(LogJafgInternal, "Stacktrace:\n{}.", TraceStream.str())
    }
    Jafg::FlushOutStreams();

    ///////////////////////////////////////////////////////////////////////////////
    // Dialog
#if WITH_LOCAL_LAYER
    if (Application::Private::bGSuppressCrashDialog == false && Hal::IsTracerPidValidVerySlow() == false)
    {
        const LWString Caption{LITERAL_WIDE("Jafg panicked; We are fucked.")};
        const auto Message{algo::utf8_to_utf16(InMessage, std::strlen(InMessage))};

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
    Jafg::FlushOutStreams();

    ///////////////////////////////////////////////////////////////////////////////
    // The final absolute end.
    if (Hal::IsTracerPidValidVerySlow())
    {
        //# The last resort if the program is being debugged. This is the end.
        JAFG_PLATFORM_BREAK()
    }

    LOnPlatformBreakWindows::ExitQuietly();
}

void Jafg::LOnPlatformBreakWindows::OnProgramPanic
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

namespace Jafg::Hal
{

void SleepNoStats(const f64 InSeconds)
{
    if (const DWORD Milli{ static_cast<DWORD>(InSeconds * maths::s2ms_d) }; Milli > 0)
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

} /* ~Namespace Jafg::Hal */

#endif /* PLATFORM_WINDOWS */
