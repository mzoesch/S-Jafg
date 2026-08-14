// Copyright mzoesch. All rights reserved.

#if JAFG_PLATFORM_WINDOWS

#include "Core/App.h"

#if JAFG_WITH_MSVC
    #if !JAFG_IN_SHIPPING
        #ifndef _DEBUG
            #erorr "_DEBUG must be defined to use <crtdbg.h>."
        #endif /* !_DEBUG */
        #include <crtdbg.h>
    #endif /* !JAFG_IN_SHIPPING */
#endif /* JAFG_WITH_MSVC */

using namespace Jafg;

extern EPlatformExit::Type AgnosticLaunch();

DECLARE_INLINE_LOG_CATEGORY(LogCRT, Trace)

namespace
{
void InvalidParameterHandler(
      LChar const* Expression
    , LChar const* Function
    , LChar const* File
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
        ErrorLevel = AgnosticLaunch();
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        HandelSeh();
    }
    return ErrorLevel;
}
#endif /* JAFG_WITH_MSVC */

#if JAFG_IN_SHIPPING
i32 WINAPI WinMain(_In_ HINSTANCE hInInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ char* pCmdLine, _In_ i32 nCmdShow)
#else /* JAFG_IN_SHIPPING */
i32 main(i32 c, char* v[])
#endif /* !JAFG_IN_SHIPPING */
{
#if JAFG_IN_SHIPPING
    i32 c{__argc};
    char** v{__argv};
#endif /* JAFG_IN_SHIPPING */

    i32 ErrorLevel{};

    check(c > 0)
    TArray<LString> Arguments; algo::for_each(v + 1, v + c, [&Arguments](auto* Arg){ Arguments.emplace_back(Arg); });
    App::Detail::RawCommandLine = std::move(Arguments);

    if (App::Detail::HasArgumentToWaitForDebuggerVeryEarlyOnly())
    {
        App::Detail::WaitForDebuggerGracefully(true);
    }

    _set_invalid_parameter_handler(::InvalidParameterHandler);

#if JAFG_WITH_MSVC
    App::Detail::AlwaysReportCrash = algo::contains(App::GetRawCommandLine(), "--Jafg.AlwaysReportCrash");
#endif /* JAFG_WITH_MSVC */

#if !JAFG_IN_SHIPPING && JAFG_WITH_MSVC
    //_CrtSetDebugFillThreshold(SIZE_MAX);
#endif /* !JAFG_IN_SHIPPING && JAFG_WITH_MSVC */

#if JAFG_WITH_MSVC
#if !JAFG_IN_SHIPPING
    if (App::Detail::IsTracerPidValidVerySlow() && !App::IsAlwaysReportCrash())
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
#endif /* !JAFG_IN_SHIPPING */
    ErrorLevel = SehUnwinder();
#else /* JAFG_WITH_MSVC */
    ErrorLevel = AgnosticLaunch();
#endif /* !JAFG_WITH_MSVC */

    if (App::IsPauseBeforeExit())
    {
        LOG_INFO(LogPlatform, "Pausing before exit.")
        LOG_INFO(LogPlatform, "Press any key to continue...")
        Jafg::FlushOutStreams();

        std::cin.get();
    }

    return ErrorLevel;
}

#endif /* JAFG_PLATFORM_WINDOWS */
