// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

namespace Logging::Detail
{

inline constexpr bool StartsWith(const char* InString, const char* InPrefix) noexcept
{
    while (*InString == *InPrefix)
    {
        if (*InPrefix == '\0')
        {
            return true;
        }

        ++InString;
        ++InPrefix;
    }

    return false;
}

inline constexpr const char* ConsumeType(const char* Begin, const char* End) noexcept
{
    static const char* const Signed("signed");
    static const char* const Unsigned("unsigned");

    const char* It = Begin;
    if (Logging::Detail::StartsWith(It, Signed))
    {
        It += ::strlen(Signed) + 1;
    }
    else if (Logging::Detail::StartsWith(It, Unsigned))
    {
        It += strlen(Unsigned) + 1;
    }

    i32 TemplateNest = 0;
    while (It != End)
    {
        if (*It == ' ' && TemplateNest == 0)
        {
            break;
        }

        if (*It == '<')
        {
            ++TemplateNest;
        }
        else if (*It == '>' && TemplateNest > 0)
        {
            --TemplateNest;
        }

        ++It;
    }

    return It;
}

} /* ~Namespace Logging::Detail */

inline constexpr std::string_view PrettyFunctionName(const char* InFunctionName) noexcept
{
    const char* Begin = InFunctionName;
    const char* End   = InFunctionName;
    while (*End != '\0')
    {
        ++End;
    }

    Begin = Logging::Detail::ConsumeType(Begin, End);
    if (Begin == End)
    {
        return InFunctionName;
    }

    /* Space after return type */
    ++Begin;
    if (Begin == End)
    {
        return InFunctionName;
    }

    if (*Begin == '(')
    {
        Begin = Logging::Detail::ConsumeType(++Begin, End);

        /* Space after return type */
        ++Begin;
        if (Begin == End)
        {
            return InFunctionName;
        }
    }

    return { Begin, static_cast<std::string_view::size_type>(std::find(Begin, End, '(') - Begin) };
}

inline constexpr LString LogColor_Trace{ JAFG_LOG_COLOR_TRACE };
inline constexpr LString LogColor_Verbose{ JAFG_LOG_COLOR_VERBOSE };
inline constexpr LString LogColor_Info{ JAFG_LOG_COLOR_INFO };
inline constexpr LString LogColor_Warning{ JAFG_LOG_COLOR_WARNING };
inline constexpr LString LogColor_Error{ JAFG_LOG_COLOR_ERROR };
inline constexpr LString LogColor_Fatal{ JAFG_LOG_COLOR_FATAL };
inline constexpr LString LogColor_End{ JAFG_LOG_COLOR_END };
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
