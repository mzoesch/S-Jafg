// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

LAL_API std::string_view PrettyFunctionName(const char* InFunctionName) noexcept;

LAL_API extern Jafg::LString LogColor_Trace;
LAL_API extern Jafg::LString LogColor_Verbose;
LAL_API extern Jafg::LString LogColor_Info;
LAL_API extern Jafg::LString LogColor_Warning;
LAL_API extern Jafg::LString LogColor_Error;
LAL_API extern Jafg::LString LogColor_Fatal;
LAL_API extern Jafg::LString LogColor_End;

LAL_API Jafg::LString    GetColorForVerbosity(const ELogVerbosity::Type InVerbosity);
LAL_API std::string_view GetColorForVerbosityView(const ELogVerbosity::Type InVerbosity);

//# Instantly flushes log output. Caller will have to wait.
FORCEINLINE void FlushOutStreams();

//#
//# Defines a log category that can be used to log messages.
//#
//# @param  Category  A human-friendly name for the category.
//# @tparam Verbosity The filter level for the category. If a message is below this level, it will not be logged
//#                   even if the category is enabled.
//#
template <ELogVerbosity::Type Verbosity>
struct LLogCategory final
{
    LLogCategory() = delete;
    PROHIBIT_REALLOC_OF_ANY_FORM(LLogCategory)
    FORCEINLINE explicit LLogCategory(std::string_view&& InCategory)
        : Category(std::move(InCategory))
    {
    }
    ~LLogCategory() = default;

    FORCEINLINE static consteval ELogVerbosity::Type GetCompileTimeVerbosity() { return Verbosity; }
    FORCEINLINE const std::string_view& GetCategory() const { return this->Category; }

private:

    const std::string_view Category;
};

FORCEINLINE void FlushOutStreams()
{
    //
    // This seems weird and it is. But we currently do not really have a
    // logger lol. We basically just use print statements everywhere but
    // hide them behind this complicated macro system to let other
    // people think we are professional and somehow ... inhuman?
    //

#if LAL_PLATFORM_SUPPORTS_STD_FLUSH
    std::cout.flush();
    std::cerr.flush();
#endif /* !LAL_PLATFORM_SUPPORTS_STD_FLUSH */

    return;
}

//# This function may be used with ANSI escapes.
template <ELogVerbosity::Type InVerbosity, ELogVerbosity::Type InCategoryVerbosity, typename ... TArgs>
FORCEINLINE void LogMessage(std::format_string<TArgs...> InFormat, TArgs&& ... InArgs)
{
#if PLATFORM_WASM
    if constexpr (ActualVerbosity == ELogVerbosity::Warning)
    {
        ::emscripten_log(EM_LOG_CONSOLE | EM_LOG_WARN, std::format(InFormat, InArgs ...));
    }
    else if constexpr (ActualVerbosity == ELogVerbosity::Error || ActualVerbosity == ELogVerbosity::Fatal)
    {
        ::emscripten_log(EM_LOG_CONSOLE | EM_LOG_ERROR, std::format(InFormat, InArgs ...));
    }
    else
    {
        ::emscripten_log(EM_LOG_CONSOLE, std::format(InFormat, InArgs ...));
    }
#else /* PLATFORM_WASM */
    if constexpr ((InVerbosity < InCategoryVerbosity) == false)
    {
        std::cout << std::format(InFormat, std::forward<TArgs>(InArgs) ...) << '\n';
    }
#endif /* !PLATFORM_WASM */

    return;
}

} /* ~Namespace Lal */
