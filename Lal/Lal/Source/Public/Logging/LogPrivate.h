// Copyright mzoesch. All rights reserved.

#pragma once

#include "LongLiquidLogger.h"

namespace Lal
{

LAL_API std::string_view PrettyFunctionName(const char* InFunctionName) noexcept;

LAL_API extern LString LogColor_Trace;
LAL_API extern LString LogColor_Verbose;
LAL_API extern LString LogColor_Info;
LAL_API extern LString LogColor_Warning;
LAL_API extern LString LogColor_Error;
LAL_API extern LString LogColor_Fatal;
LAL_API extern LString LogColor_End;

LAL_API LString     GetColorForVerbosity(const ELogVerbosity::Type InVerbosity);
LAL_API LStringView GetColorForVerbosityView(const ELogVerbosity::Type InVerbosity);

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

#if LAL_SAVE_LOGS_IN_MEMORY
template<ELogVerbosity::Type Verbosity, ELogVerbosity::Type CategoryVerbosity, typename... TArgs>
FORCEINLINE void SaveLog(std::format_string<TArgs...> Format, TArgs&&... Args)
{
    if constexpr ((Verbosity < CategoryVerbosity) == false)
    {
        std::unique_lock Lock{ JafgCore::GLongLiquidLogsMutex };
        JafgCore::GLongLiquidLogs.emplace_back
        (
            std::make_tuple
            (
                LexToString(Verbosity),
                std::string{std::format(Format, std::forward<TArgs>(Args)...)}
            )
        );
    }

    return;
}
#endif /* LAL_SAVE_LOGS_IN_MEMORY */

template<ELogVerbosity::Type Verbosity, ELogVerbosity::Type CategoryVerbosity, typename... TArgs>
FORCEINLINE void LogMessage(std::format_string<TArgs...> Format, TArgs&&... Args)
{
    /* Not thread safe. But who actually care. It's just logs. */
#if PLATFORM_WASM
    if constexpr ((Verbosity < CategoryVerbosity) == false)
    {
        if constexpr (Verbosity == ELogVerbosity::Warning)
        {
            ::emscripten_log(EM_LOG_CONSOLE | EM_LOG_WARN, std::format(Format, Args...));
        }
        else if constexpr (Verbosity == ELogVerbosity::Error || Verbosity == ELogVerbosity::Fatal)
        {
            ::emscripten_log(EM_LOG_CONSOLE | EM_LOG_ERROR, std::format(Format, Args...));
        }
        else
        {
            ::emscripten_log(EM_LOG_CONSOLE, std::format(Format, Args...));
        }
    }
#else /* PLATFORM_WASM */
    if constexpr ((Verbosity < CategoryVerbosity) == false)
    {
        std::cout << std::format(Format, std::forward<TArgs>(Args)...) << '\n';
    }
#endif /* !PLATFORM_WASM */

    return;
}

} /* ~Namespace Lal */
