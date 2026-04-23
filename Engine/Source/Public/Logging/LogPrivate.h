// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

inline LString GetPrettyFunctionName(LStringView FunctionName) noexcept
{
    LString Result; Result.reserve(FunctionName.size());

    std::size_t Stack{};
    for (auto It{FunctionName.begin()}; It != FunctionName.end(); ++It)
    {
        if (*It == '<')
        {
            ++Stack;
            continue;
        }
        if (*It == '>')
        {
            --Stack;
            continue;
        }
        if (Stack > 0)
        {
            continue;
        }

        Result += *It;
    }

    return Result;
}

inline constexpr LStringView LogColor_Trace{ JAFG_LOG_COLOR_TRACE };
inline constexpr LStringView LogColor_Verbose{ JAFG_LOG_COLOR_VERBOSE };
inline constexpr LStringView LogColor_Info{ JAFG_LOG_COLOR_INFO };
inline constexpr LStringView LogColor_Warning{ JAFG_LOG_COLOR_WARNING };
inline constexpr LStringView LogColor_Error{ JAFG_LOG_COLOR_ERROR };
inline constexpr LStringView LogColor_Fatal{ JAFG_LOG_COLOR_FATAL };
inline constexpr LStringView LogColor_End{ JAFG_LOG_COLOR_END };
inline constexpr LStringView GetColorForVerbosity(const ELogVerbosity::Type Verbosity)
{
    if (Verbosity == ELogVerbosity::Trace) { return LogColor_Trace; }
    if (Verbosity == ELogVerbosity::Verbose) { return LogColor_Verbose; }
    if (Verbosity == ELogVerbosity::Info) { return LogColor_Info; }
    if (Verbosity == ELogVerbosity::Warning) { return LogColor_Warning; }
    if (Verbosity == ELogVerbosity::Error) { return LogColor_Error; }
    if (Verbosity == ELogVerbosity::Fatal) { return LogColor_Fatal; }
    return {};
}

//# Instantly flushes log output. Caller will have to wait.
inline void FlushOutStreams() noexcept
{
    //
    // This seems weird and it is. But we currently do not really have a
    // logger lol. We basically just use print statements everywhere but
    // hide them behind this complicated macro system to let other
    // people think we are professional and somehow ... inhuman?
    //

#if JAFG_PLATFORM_SUPPORTS_STD_FLUSH
    std::cout.flush();
    std::cerr.flush();
#endif /* !JAFG_PLATFORM_SUPPORTS_STD_FLUSH */

    return;
}

//#
//# Defines a log category that can be used to log messages.
//#
//# @param  Category  A human-friendly name for the category.
//# @tparam Verbosity The filter level for the category. If a message is below this level, it will not be logged
//#                   even if the category is enabled.
//#
template<ELogVerbosity::Type Verbosity>
struct LLogCategory final
{
    LLogCategory() noexcept = delete;
    PROHIBIT_REALLOC_OF_ANY_FORM(LLogCategory)
    inline constexpr explicit LLogCategory(std::string_view InCategory) noexcept : Category(InCategory) {}
    ~LLogCategory() = default;

    inline static consteval ELogVerbosity::Type GetCompileTimeVerbosity() noexcept { return Verbosity; }
    inline std::string_view GetCategory() const noexcept { return this->Category; }

private:

    const std::string_view Category;
};

#if JAFG_SAVE_LOGS_IN_MEMORY

namespace Detail
{

ENGINE_API extern std::mutex GLongLiquidLogsMutex;
ENGINE_API extern std::vector<std::tuple<
    std::string, /* Verbosity */
    std::string  /* Message */
    >> GLongLiquidLogs;

} /* ~Namespace Detail */

template<ELogVerbosity::Type Verbosity, ELogVerbosity::Type CategoryVerbosity, typename... TArgs>
inline void SaveLog(std::format_string<TArgs...> Format, TArgs&&... Args) noexcept
{
    if constexpr ((Verbosity < CategoryVerbosity) == false)
    {
        std::unique_lock Lock{Jafg::Detail::GLongLiquidLogsMutex};
        Jafg::Detail::GLongLiquidLogs.emplace_back(std::make_tuple(
            LexToString(Verbosity),
            std::string{std::format(Format, std::forward<TArgs>(Args)...)}
            ));
    }

    return;
}
#endif /* JAFG_SAVE_LOGS_IN_MEMORY */

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

} /* ~Namespace Jafg */
