// Copyright mzoesch. All rights reserved.

#if PLATFORM_WINDOWS

#include "Core/Application.h"
#include "Platform/PlatformMisc.h"

#if JAFG_WITH_MSVC
    #if !IN_SHIPPING
        #ifndef _DEBUG
            #erorr "_DEBUG must be defined to use <crtdbg.h>."
        #endif /* !_DEBUG */
        #include <crtdbg.h>
    #endif /* !IN_SHIPPING */
#endif /* JAFG_WITH_MSVC */

using namespace Jafg;

extern EPlatformExit::Type GuardedMain();

DECLARE_INLINE_LOG_CATEGORY(LogCRT, Trace)

namespace
{
void InvalidParameterHandler(
      const LChar* Expression
    , const LChar* Function
    , const LChar* File
    , u32 Line
    , u64 pReserved
    )
{
    LString Utf8Expression{algo::utf16_to_utf8(Expression, std::wcslen(Expression))};
    LString Utf8Function{algo::utf16_to_utf8(Function, std::wcslen(Function))};
    LString Utf8File{algo::utf16_to_utf8(File, std::wcslen(File))};

    LOG_FATAL(LogCRT, "Invalid parameter detected inside expression:\n\t{}\nFunction {}\nFile: {}\nLine: {}",
        Utf8Expression,
        Utf8Function,
        Utf8File,
        Line
        )
}
} /* ~Namespace <Anonymous> */

#if JAFG_WITH_MSVC
NORETURN
void HandelSeh()
{
    LOG_FATAL(LogPlatform, "Unhandled exception thrown.")
}

EPlatformExit::Type SehUnwinder()
{
    EPlatformExit::Type ErrorLevel{};
    __try
    {
        ErrorLevel = GuardedMain();
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        HandelSeh();
    }
    return ErrorLevel;
}
#endif /* JAFG_WITH_MSVC */

#if IN_SHIPPING
i32 WINAPI WinMain(_In_ HINSTANCE hInInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ char* pCmdLine, _In_ i32 nCmdShow)
#else /* IN_SHIPPING */
i32 main(i32 ArgC, char* ArgV[])
#endif /* !IN_SHIPPING */
{
#if IN_SHIPPING
    i32 ArgC{__argc};
    char** ArgV{__argv};
#endif /* IN_SHIPPING */

    i32 ErrorLevel{};

    TArray<LString> Arguments;
    for (auto Idx{0uz}; Idx < static_cast<LSize>(ArgC); ++Idx)
    {
        Arguments.emplace_back(ArgV[Idx]);
    }
    Application::Private::RawCommandLine = std::move(Arguments);

    if (algo::contains(Application::GetRawCmdLine(), "-Jafg.WaitForDebugger"))
    {
        Application::Private::WaitForDebuggerGracefully(true);
    }

    _set_invalid_parameter_handler(::InvalidParameterHandler);

#if JAFG_WITH_MSVC
    Application::Private::bAlwaysReportCrash = Application::HasCmdLineParameter("AlwaysReportCrash");
#endif /* JAFG_WITH_MSVC */

#if !IN_SHIPPING && JAFG_WITH_MSVC
    //_CrtSetDebugFillThreshold(SIZE_MAX);
#endif /* !IN_SHIPPING && JAFG_WITH_MSVC */

#if JAFG_WITH_MSVC
#if !IN_SHIPPING
    if (Application::HasTracerPidNow() && (Application::IsAlwaysReportCrash() == false))
    {
        LOG_VERBOSE(LogPlatform, "Suppressing crash dialog due to presence of tracer pid.")
        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
        _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
    }
    else
    {
        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_WNDW);
        _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_WNDW);
    }
#endif /* !IN_SHIPPING */
    ErrorLevel = SehUnwinder();
#else /* JAFG_WITH_MSVC */
    ErrorLevel = GuardedMain();
#endif /* !JAFG_WITH_MSVC */

    if (Application::IsPauseBeforeExit())
    {
        LOG_INFO(LogPlatform, "Pausing before exit.")
        LOG_INFO(LogPlatform, "Press any key to continue...")
        Jafg::FlushOutStreams();

        std::cin.get();
    }

    return ErrorLevel;
}

#endif /* PLATFORM_WINDOWS */
