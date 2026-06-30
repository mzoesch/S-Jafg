// Copyright mzoesch. All rights reserved.

#if JAFG_PLATFORM_WINDOWS

#include "Minimal.afx"
#include "Core/App.h"

#include <dbghelp.h>
#include <csignal>

void Jafg::App::Detail::TrapMeFn::operator()() const noexcept
{
    ::_Exit(EXIT_FAILURE);
}

void Jafg::App::Detail::TrapMeFn::operator()(std::string_view Message) const noexcept
{
    const auto Proc{::GetCurrentProcess()};
    const auto Pid{::GetCurrentProcessId()};
    const auto Tid{::GetCurrentThreadId()};

    auto hFile{::CreateFileW(finder::detail::dump_file.c_str()
        , GENERIC_WRITE
        , 0
        , nullptr
        , CREATE_ALWAYS
        , FILE_ATTRIBUTE_NORMAL
        , nullptr
        )};
    if (hFile != INVALID_HANDLE_VALUE)
    {
        if (::MiniDumpWriteDump(
            Proc, Pid, hFile,
            static_cast<MINIDUMP_TYPE>(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory), nullptr, nullptr, nullptr
            ) == FALSE)
        {
            LOG_ERROR(LogPlatform,
                "Failed to create memory dump for our process [PID: [{}] with TID: [{}]]. Platform error: [{}]."
                , Pid, Tid, ::GetLastError()
                )
        }
        ::CloseHandle(hFile);
    }
    else
    {
        // Immediate elevation. Not our problem.
        std::abort();
    }

    LOG_ERROR(LogJafgInternal, "Fatal Error: [{}].", Message)

    if (App::Detail::bDumpStack)
    {
        void* Stack[JAFG_PLATFORM_MAX_FRAMES];
        USHORT Frames{::CaptureStackBackTrace(0, JAFG_PLATFORM_MAX_FRAMES, Stack, nullptr)};

        if (::SymInitialize(::GetCurrentProcess(), nullptr, TRUE))
        {
            SYMBOL_INFO* Symbol{static_cast<SYMBOL_INFO*>(calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1))};
            Symbol->MaxNameLen = 255;
            Symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

            IMAGEHLP_LINE64 Line;
            Line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

            DWORD Displacement;
            for (USHORT Idx{0uz}; Idx < Frames; ++Idx)
            {
                DWORD64 Addr{reinterpret_cast<DWORD64>(Stack[Idx])};
                if (::SymFromAddr(Proc, Addr, nullptr, Symbol))
                {
                    if (::SymGetLineFromAddr64(Proc, Addr, &Displacement, &Line))
                    {
                        printf("#%02u %s (%s:%lu)\n", Idx, Symbol->Name, Line.FileName, Line.LineNumber);
                    }
                    else
                    {
                        printf("#%02u %s (no line info)\n", Idx, Symbol->Name);
                    }
                }
                else
                {
                    printf("#%02u [unknown]\n", Idx);
                }
            }
            ::free(Symbol);
            ::SymCleanup(Proc);
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
    }

    Jafg::FlushOutStreams();

#if JAFG_WITH_LOCAL_LAYER
    if (!App::Detail::bSuppressCrashDialog && !IsTracerPidValidVerySlow())
    {
        MessageBox(nullptr, algo::utf8_to_utf16(Message).c_str(), LITERAL_WIDE("Jafg panicked; We are fucked."), MB_ICONERROR | MB_OK);
    }
    else
    {
        LOG_VERBOSE(LogJafgInternal, "Suppressed jafg crash dialog window.")
    }
#endif /* JAFG_WITH_LOCAL_LAYER */

    // Flush, because some streams may be buffered and missing while aborting.
    Jafg::FlushOutStreams();

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
    if (const DWORD Milli{ static_cast<DWORD>(InSeconds * maths::s2ms_d) }; Milli > 0)
    {
        ::Sleep(Milli);
    }
    else
    {
        std::this_thread::yield();
    }
}

bool Jafg::App::Detail::IsTracerPidValidVerySlow()
{
    return ::IsDebuggerPresent();
}

#endif /* JAFG_PLATFORM_WINDOWS */
