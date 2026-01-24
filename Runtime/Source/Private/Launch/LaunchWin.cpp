// Copyright mzoesch. All rights reserved.

#if PLATFORM_WINDOWS

#include "Core/Application.h"
#include "Platform/PlatformMisc.h"
#include "AbsoluteMinimalCore.h"

#if JAFG_WITH_MSVC
    #if IN_DEBUG
        #ifndef _DEBUG
            #erorr "_DEBUG must be defined to use <crtdbg.h>."
        #endif /* !_DEBUG */
        #include <crtdbg.h>
    #endif /* IN_DEBUG */
#endif /* JAFG_WITH_MSVC */

using namespace Jafg;

extern EPlatformExit::Type GuardedMain();

DECLARE_INLINE_LOG_CATEGORY(LogCRT, Trace)

namespace
{

void InvalidParameterHandler(
     const wchar_t* Expression
   , const wchar_t* Function
   , const wchar_t* File
   , unsigned int Line
   , uintptr_t pReserved
   )
{
    LString Utf8Expression{ Jafg::Utf16ToUtf8(Expression, std::wcslen(Expression)) };
    LString Utf8Function{ Jafg::Utf16ToUtf8(Function, std::wcslen(Function)) };
    LString Utf8File{ Jafg::Utf16ToUtf8(File, std::wcslen(File)) };

    LOG_FATAL(LogCRT, "Invalid parameter detected inside expression:\n\t{}\nFunction {}\nFile: {}\nLine: {}",
        Utf8Expression,
        Utf8Function,
        Utf8File,
        Line
        )
}

} /* ~Namespace <Anonymous> */

i32 WINAPI WinMain(_In_ HINSTANCE hInInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ char* pCmdLine, _In_ i32 nCmdShow)
{
    //
    // If LNK2019 [int __cdecl __scrt_common_main_seh(void)] make sure to set the System-Linker of the Runtime
    // Project to use the subsystem "Not Set" (for automatic platform detection) or "Windows".
    //
    i32 ErrorLevel = 0;

    TArray<LString> Arguments;
    for (i32 Idx{ 0 }; Idx < __argc; ++Idx)
    {
        Arguments.emplace_back(__argv[Idx]);
    }
    Application::Private::RawCommandLine = std::move(Arguments);

    if (algo::contains(Application::GetRawCmdLine(), "-WaitForDebugger"))
    {
        Application::Private::WaitForDebuggerGracefully(true);
    }

    _set_invalid_parameter_handler(::InvalidParameterHandler);
#if IN_DEBUG && JAFG_WITH_MSVC
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
    _CrtSetDebugFillThreshold(0);
#endif /* IN_DEBUG && JAFG_WITH_MSVC */

#if JAFG_WITH_MSVC
    Application::Private::bAlwaysReportCrash = Application::HasCmdLineParameter("AlwaysReportCrash");
#endif /* JAFG_WITH_MSVC */

#if JAFG_WITH_MSVC
    if (Application::HasTracerPidNow() && (Application::IsAlwaysReportCrash() == false))
    {
        LOG_VERBOSE(LogPlatform, "Suppressing crash dialog due to presence of tracer pid.")
        ErrorLevel = GuardedMain();
    }
    else
    {
        LOG_VERBOSE(LogPlatform, "Leveraging structured exception handling.")

        __try
        {
#endif /* JAFG_WITH_MSVC */
            ErrorLevel = GuardedMain();
#if JAFG_WITH_MSVC
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            LOG_FATAL(LogPlatform, "Unhandled exception thrown.")
        }
    }
#endif /* JAFG_WITH_MSVC */

    if (Application::IsPauseBeforeExit())
    {
        LOG_INFO(LogPlatform, "Pausing before exit.")
        LOG_INFO(LogPlatform, "Press any key to continue...")
        JAFG_UNSAFE_FLUSH_OUT_STREAMS()

        std::cin.get();
    }

    return ErrorLevel;
}

#endif /* PLATFORM_WINDOWS */
