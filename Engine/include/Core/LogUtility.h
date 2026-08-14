// Copyright mzoesch. All rights reserved.

#pragma once

//# All verbosity levels jafg supports.
enum struct ELogVerbosity: u8
{
    Trace,
    Verbose,
    Info,
    Warning,
    Error,
    Fatal,
};

namespace Jafg
{

//# Default verbosity used for debug builds.
#ifndef JAFG_DEBUG_DEFAULT_LOG_VERBOSITY
    #define JAFG_DEBUG_DEFAULT_LOG_VERBOSITY             Trace
#endif /* JAFG_DEBUG_DEFAULT_LOG_VERBOSITY */
//# Default verbosity used for development builds.
#ifndef JAFG_DEVELOPMENT_DEFAULT_LOG_VERBOSITY
    #define JAFG_DEVELOPMENT_DEFAULT_LOG_VERBOSITY       Verbose
#endif /* JAFG_DEVELOPMENT_DEFAULT_LOG_VERBOSITY */
//# Default verbosity used for shipping builds.
#ifndef JAFG_SHIPPING_DEFAULT_LOG_VERBOSITY
    #define JAFG_SHIPPING_DEFAULT_LOG_VERBOSITY          Info
#endif /* JAFG_SHIPPING_DEFAULT_LOG_VERBOSITY */

//# The current default log verbosity.
#ifndef JAFG_LOG_DEFAULT_VERBOSITY
    #if JAFG_IN_DEBUG
        #define JAFG_LOG_DEFAULT_VERBOSITY               JAFG_DEBUG_DEFAULT_LOG_VERBOSITY
    #elif JAFG_IN_DEVELOPMENT
        #define JAFG_LOG_DEFAULT_VERBOSITY               JAFG_DEVELOPMENT_DEFAULT_LOG_VERBOSITY
    #elif JAFG_IN_SHIPPING
        #define JAFG_LOG_DEFAULT_VERBOSITY               JAFG_SHIPPING_DEFAULT_LOG_VERBOSITY
    #else /* JAFG_IN_SHIPPING */
        #error "Could not resolve build configuration."
    #endif  /* !JAFG_IN_SHIPPING */
#endif /* !JAFG_LOG_DEFAULT_VERBOSITY */

//#
//# These macro changes if any logging category can log in said verbosity level. Enabling a lower category
//# automatically enables all higher categories.
//#
#ifndef JAFG_LOG_ENABLE_TRACE
    #define JAFG_LOG_ENABLE_TRACE                (JAFG_IN_DEBUG)
#endif /* JAFG_LOG_ENABLE_TRACE */
#ifndef JAFG_LOG_ENABLE_VERBOSE
    #define JAFG_LOG_ENABLE_VERBOSE              ((JAFG_LOG_ENABLE_TRACE) || (JAFG_IN_DEBUG || JAFG_IN_DEVELOPMENT))
#endif /* JAFG_LOG_ENABLE_VERBOSE */
#ifndef JAFG_LOG_ENABLE_INFO
    #define JAFG_LOG_ENABLE_INFO                 ((JAFG_LOG_ENABLE_VERBOSE) || (!JAFG_IN_SHIPPING))
#endif /* JAFG_LOG_ENABLE_INFO */
#ifndef JAFG_LOG_ENABLE_WARNING
    #define JAFG_LOG_ENABLE_WARNING              ((JAFG_LOG_ENABLE_INFO) || (!JAFG_IN_SHIPPING))
#endif /* JAFG_LOG_ENABLE_WARNING */
#ifndef JAFG_LOG_ENABLE_ERROR
    #define JAFG_LOG_ENABLE_ERROR                ((JAFG_LOG_ENABLE_WARNING) || (!JAFG_IN_SHIPPING))
#endif /* JAFG_LOG_ENABLE_ERROR */

//#
//# Create an inline log category inside a header to transitively enable logging for this category
//# in all translation units that include said header.
//#
//# @param Category  A human-friendly name for the category.
//# @param Verbosity The filter level for the category. If a message is below this level, it will not be logged
//#                  even if the category is enabled.
//#
#define DECLARE_INLINE_LOG_CATEGORY(Category, Verbosity) \
    namespace LogDetail{ struct JAFG_JOIN_OUTER_THREE(_, Category, _Type) {}; } \
    inline ::Jafg::TLogCategory<::ELogVerbosity::Verbosity, LogDetail::JAFG_JOIN_OUTER_THREE(_, Category, _Type)> Category;

//#
//# Defines a log category that can be used to log messages.
//#
//# @param  Category A human-friendly name for the category.
//# @tparam V        The filter level for the category. If a message is below this level, it will not be logged
//#                  even if the category is enabled.
//#
template<ELogVerbosity V, typename T>
struct TLogCategory final
{
    static constexpr ::ELogVerbosity Verbosity{V};
    template<::ELogVerbosity RequestedVerbosity>
    static constexpr bool CompilesFor{std::to_underlying(RequestedVerbosity) >= std::to_underlying(Verbosity)};
};

//# Optional ANSI colors.
#if JAFG_PLATFORM_SUPPORTS_ANSI_ESCAPES
    #ifndef JAFG_LOG_COLOR_TRACE
        #define JAFG_LOG_COLOR_TRACE            "\033[;90m"      /* Dark gray   */
    #endif /* !JAFG_LOG_COLOR_TRACE */
    #ifndef JAFG_LOG_COLOR_VERBOSE
        #define JAFG_LOG_COLOR_VERBOSE          "\033[;37m"      /* Light gray  */
    #endif /* !JAFG_LOG_COLOR_VERBOSE */
    #ifndef JAFG_LOG_COLOR_INFO
        #define JAFG_LOG_COLOR_INFO             "\033[;97m"      /* White       */
    #endif /* !JAFG_LOG_COLOR_INFO */
    #ifndef JAFG_LOG_COLOR_WARNING
        #define JAFG_LOG_COLOR_WARNING          "\033[;93m"      /* Yellow      */
    #endif /* !JAFG_LOG_COLOR_WARNING */
    #ifndef JAFG_LOG_COLOR_ERROR
        #define JAFG_LOG_COLOR_ERROR            "\033[;91m"      /* Red         */
    #endif /* !JAFG_LOG_COLOR_ERROR */
    #ifndef JAFG_LOG_COLOR_FATAL
        #define JAFG_LOG_COLOR_FATAL            "\033[;31m"      /* Bright red  */
    #endif /* !JAFG_LOG_COLOR_FATAL */
    #ifndef JAFG_LOG_COLOR_END
        #define JAFG_LOG_COLOR_END              "\033[0m"        /* Reset color */
    #endif /* !JAFG_LOG_COLOR_END */
#else /* JAFG_PLATFORM_SUPPORTS_ANSI_ESCAPES */
    #ifndef JAFG_LOG_COLOR_TRACE
        #define JAFG_LOG_COLOR_TRACE            ""
    #endif /* !JAFG_LOG_COLOR_TRACE */
    #ifndef JAFG_LOG_COLOR_VERBOSE
        #define JAFG_LOG_COLOR_VERBOSE          ""
    #endif /* !JAFG_LOG_COLOR_VERBOSE */
    #ifndef JAFG_LOG_COLOR_INFO
        #define JAFG_LOG_COLOR_INFO             ""
    #endif /* !JAFG_LOG_COLOR_INFO */
    #ifndef JAFG_LOG_COLOR_WARNING
        #define JAFG_LOG_COLOR_WARNING          ""
    #endif /* !JAFG_LOG_COLOR_WARNING */
    #ifndef JAFG_LOG_COLOR_ERROR
        #define JAFG_LOG_COLOR_ERROR            ""
    #endif /* !JAFG_LOG_COLOR_ERROR */
    #ifndef JAFG_LOG_COLOR_FATAL
        #define JAFG_LOG_COLOR_FATAL            ""
    #endif /* !JAFG_LOG_COLOR_FATAL */
    #ifndef JAFG_LOG_COLOR_END
        #define JAFG_LOG_COLOR_END              ""
    #endif /* !JAFG_LOG_COLOR_END */
#endif /* !JAFG_PLATFORM_SUPPORTS_ANSI_ESCAPES */

inline constexpr LStringView LogColor_Trace{ JAFG_LOG_COLOR_TRACE };
inline constexpr LStringView LogColor_Verbose{ JAFG_LOG_COLOR_VERBOSE };
inline constexpr LStringView LogColor_Info{ JAFG_LOG_COLOR_INFO };
inline constexpr LStringView LogColor_Warning{ JAFG_LOG_COLOR_WARNING };
inline constexpr LStringView LogColor_Error{ JAFG_LOG_COLOR_ERROR };
inline constexpr LStringView LogColor_Fatal{ JAFG_LOG_COLOR_FATAL };
inline constexpr LStringView LogColor_End{ JAFG_LOG_COLOR_END };
NODISCARD constexpr LStringView GetColorForVerbosity(::ELogVerbosity Verbosity)
{
    switch (Verbosity)
    {
        case ::ELogVerbosity::Trace:   return LogColor_Trace;
        case ::ELogVerbosity::Verbose: return LogColor_Verbose;
        case ::ELogVerbosity::Info:    return LogColor_Info;
        case ::ELogVerbosity::Warning: return LogColor_Warning;
        case ::ELogVerbosity::Error:   return LogColor_Error;
        case ::ELogVerbosity::Fatal:   return LogColor_Fatal;
        default: return {};
    }
}

//#
//# A macro that logs a formatted message if the log category is active at TRACE verbosity level.
//#
//# @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
//# @param Format       Format string literal in the style of std::format.
//#
#if JAFG_LOG_ENABLE_TRACE
    #define LOG_TRACE(Category, Format, ...) \
        DETAIL_JAFG_LOG(Category, ::ELogVerbosity::Trace, Format, __VA_ARGS__)
#else /* JAFG_LOG_ENABLE_TRACE */
    #define LOG_TRACE(Category, Format, ...)
#endif /* !JAFG_LOG_ENABLE_TRACE */

//#
//# A macro that logs a formatted message if the log category is active at VERBOSE verbosity level.
//#
//# @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
//# @param Format       Format string literal in the style of std::format.
//#
#if JAFG_LOG_ENABLE_VERBOSE
    #define LOG_VERBOSE(Category, Format, ...) \
        DETAIL_JAFG_LOG(Category, ::ELogVerbosity::Verbose, Format, __VA_ARGS__)
#else /* JAFG_LOG_ENABLE_VERBOSE */
    #define LOG_VERBOSE(Category, Format, ...)
#endif /* !JAFG_LOG_ENABLE_VERBOSE */

//#
//# A macro that logs a formatted message if the log category is active at INFO verbosity level.
//#
//# @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
//# @param Format       Format string literal in the style of std::format.
//#
#if JAFG_LOG_ENABLE_INFO
    #define LOG_INFO(Category, Format, ...) \
        DETAIL_JAFG_LOG(Category, ::ELogVerbosity::Info, Format, __VA_ARGS__)
#else /* JAFG_LOG_ENABLE_INFO */
    #define LOG_INFO(Category, Format, ...)
#endif /* !JAFG_LOG_ENABLE_INFO */

//#
//# A macro that logs a formatted message if the log category is active at WARNING verbosity level.
//#
//# @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
//# @param Format       Format string literal in the style of std::format.
//#
#if JAFG_LOG_ENABLE_WARNING
    #define LOG_WARNING(Category, Format, ...) \
        DETAIL_JAFG_LOG(Category, ::ELogVerbosity::Warning, Format, __VA_ARGS__)
#else /* JAFG_LOG_ENABLE_WARNING */
    #define LOG_WARNING(Category, Format, ...)
#endif /* !JAFG_LOG_ENABLE_WARNING */

//#
//# A macro that logs a formatted message if the log category is active at ERROR verbosity level.
//#
//# @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
//# @param Format       Format string literal in the style of std::format.
//#
#if JAFG_LOG_ENABLE_ERROR
    #define LOG_ERROR(Category, Format, ...) \
        DETAIL_JAFG_LOG(Category, ::ELogVerbosity::Error, Format, __VA_ARGS__)
#else /* JAFG_LOG_ENABLE_ERROR */
    #define LOG_ERROR(Category, Format, ...)
#endif /* !JAFG_LOG_ENABLE_ERROR */

//#
//# A macro that logs a formatted message if the log category is active at FATAL verbosity level.
//#
//# @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
//# @param Format       Format string literal in the style of std::format.
//#
#define LOG_FATAL(Category, Format, ...)  \
    (void)Category.Verbosity; /* Statement exists for Intellisense reasons only. */ \
    JAFG_GORGEOUS_TRAP_MSG(algo::sprintf( \
        "[" #Category "] - {}: " Format "", JAFG_PRETTY_FUNCTION_NAME __VA_OPT__(,) __VA_ARGS__))

NODISCARD inline LString GetPrettyFunctionNameSlow(LStringView FunctionName) noexcept
{
    LString Result;
    Result.reserve(FunctionName.size());

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

#if (JAFG_WITH_GCC || JAFG_WITH_CLANG)
    #define JAFG_FUNCTION_NAME                                          __FUNCTION__
    #define JAFG_FUNCTION_SIG                                           __PRETTY_FUNCTION__
#elif JAFG_WITH_MSVC
    #define JAFG_FUNCTION_NAME                                          __FUNCTION__
    #define JAFG_FUNCTION_SIG                                           __FUNCSIG__
    #define JAFG_FUNCTION_MANGLED                                       __FUNCDNAME__
#else /* JAFG_WITH_MSVC */
    #error "Compiler missing implementation."
#endif /* !JAFG_WITH_MSVC */

//# Pretty function name as [MyNameSpace::MyClass::MyFunction].
#define JAFG_PRETTY_FUNCTION_NAME                                        (::Jafg::GetPrettyFunctionNameSlow(JAFG_FUNCTION_NAME))

//# Current class name.
#define DETAIL_JAFG_LOG_TRACE_STR_CUR_CLASS                             (std::string(__FUNCTION__)

//# Current function name.
#define DETAIL_JAFG_LOG_TRACE_STR_CUR_FUNC                              (std::string(JAFG_REAL_FUNC_SIG))

//# Current function signature.
#define DETAIL_JAFG_LOG_TRACE_STR_CUR_FUNC_SIG                          (std::string(JAFG_FUNCTION_SIG))

//# Current line number.
#define DETAIL_JAFG_LOG_TRACE_STR_CUR_LINE                              (std::to_string(__LINE__))

//# Current class and function name.
#define DETAIL_JAFG_LOG_TRACE_STR_CUR_CLASS_FUNC                        (std::string(__FUNCTION__))

//# Current class name, function name and line number.
#define DETAIL_JAFG_LOG_TRACE_STR_CUR_CLASS_FUNC_LINE \
    (DETAIL_JAFG_LOG_TRACE_STR_CUR_FUNC + LITERAL_WIDE([) + DETAIL_JAFG_LOG_TRACE_STR_CUR_LINE + LITERAL_WIDE(]))

namespace Detail
{

//# Use sparingly.
ENGINE_API void EmitLogsToStdout() noexcept;
//# Use sparingly.
ENGINE_API void TryFlushStdout() noexcept;
//# Use sparingly.
ENGINE_API void EmitAndFlushLogs() noexcept;

struct LLogMessage final
{
    ELogVerbosity Verbosity;
    LString Category;
    LString Message;
};

} /* ~Namespace Detail */

template<typename TCategory, ::ELogVerbosity Verbosity, typename... TArgs>
FORCEINLINE void EmitLog(LString Category, char const* Format, TArgs&&... Args) noexcept;
#define DETAIL_JAFG_LOG(Category, Verbosity, Format, ...) \
    ::Jafg::EmitLog<decltype(Category), Verbosity>(#Category, "[" #Category "] - {}: " Format "", JAFG_PRETTY_FUNCTION_NAME __VA_OPT__(,) __VA_ARGS__);

#ifndef JAFG_LOG_DO_SCOPED_TIME_TASK_MEASURER
    #if JAFG_IN_SHIPPING
        #define JAFG_LOG_DO_SCOPED_TIME_TASK_MEASURER                    0
    #else /* JAFG_IN_SHIPPING */
        #define JAFG_LOG_DO_SCOPED_TIME_TASK_MEASURER                    1
    #endif /* !JAFG_IN_SHIPPING */
#endif /* !JAFG_LOG_DO_SCOPED_TIME_TASK_MEASURER */

//#
//# Quick and dirty way to measure the time taken by a control path. The result is being logged.
//#
//# @param Category  The category to log to.
//# @param Verbosity The verbosity level of the message. If the verbosity level of the category is lower than this,
//#                  the message will not be logged.
//# @param Name      The base name identifying the task that is being measured.
//#
#if JAFG_LOG_DO_SCOPED_TIME_TASK_MEASURER
    #define LOG_SCOPED_DURATION(Category, Verbosity, Format, ...)             \
        const ::Jafg::Detail::TScopedTimeLogger<decltype(Category),::ELogVerbosity::Verbosity> JAFG_JOIN_INNER_TWO(_ScopedTimeLogger, __COUNTER__){ \
            #Category, DETAIL_JAFG_LOG_TRACE_STR_CUR_CLASS_FUNC, \
            ::algo::sprintf(Format __VA_OPT__(,) __VA_ARGS__) };
#else /* DO_LOG_SCOPED_TIME_TASK_MEASURER */
    #define LOG_SCOPED_DURATION(Category, Verbosity, name)
#endif /* !DO_LOG_SCOPED_TIME_TASK_MEASURER */

namespace Detail
{

template<typename T, ELogVerbosity Verbosity, typename TClock = std::chrono::steady_clock>
struct TScopedTimeLogger final
{
    inline TScopedTimeLogger(LString Category, LString Function, LString BaseMessage)
        : StartTime{TClock::now()}
        , Category{std::move(Category)}
        , Function{std::move(Function)}
        , BaseMessage{std::move(BaseMessage)}
    {
    }

    ~TScopedTimeLogger()
    {
        EmitLog<T,Verbosity>(this->Category, "[{}] - {}: {} took {} seconds."
            , this->Category, this->Function
            , this->BaseMessage
            , std::chrono::duration<f64>(TClock::now() - this->StartTime).count()
            );
    }

private:

    TClock::time_point StartTime;
    LString Category;
    LString Function;
    LString BaseMessage;
};

} /* ~Namespace Detail */

} /* ~Namespace Jafg */
