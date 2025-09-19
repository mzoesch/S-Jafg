// Copyright mzoesch. All rights reserved.

#pragma once

/*----------------------------------------------------------------------------
    Quick switches.
----------------------------------------------------------------------------*/

//#
//# These macro changes if any logging category can log in said verbosity level.
//#
#ifndef LAL_LOG_ENABLE_TRACE
    #define LAL_LOG_ENABLE_TRACE                (IN_DEBUG)
#endif /* LAL_LOG_ENABLE_TRACE */
#ifndef LAL_LOG_ENABLE_VERBOSE
    #define LAL_LOG_ENABLE_VERBOSE              (IN_DEBUG || IN_DEVELOPMENT)
#endif /* LAL_LOG_ENABLE_VERBOSE */
#ifndef LAL_LOG_ENABLE_INFO
    #define LAL_LOG_ENABLE_INFO                 1
#endif /* LAL_LOG_ENABLE_INFO */
#ifndef LAL_LOG_ENABLE_WARNING
    #define LAL_LOG_ENABLE_WARNING              1
#endif /* LAL_LOG_ENABLE_WARNING */
#ifndef LAL_LOG_ENABLE_ERROR
    #define LAL_LOG_ENABLE_ERROR                1
#endif /* LAL_LOG_ENABLE_ERROR */


/*----------------------------------------------------------------------------
    Logging colors.
----------------------------------------------------------------------------*/

#if LAL_PLATFORM_SUPPORTS_ANSI_ESCAPES

    #define LAL_LOG_COLOR_TRACE            "\033[;90m"      /* Dark gray   */
    #define LAL_LOG_COLOR_VERBOSE          "\033[;37m"      /* Light gray  */
    #define LAL_LOG_COLOR_INFO             "\033[;97m"      /* White       */
    #define LAL_LOG_COLOR_WARNING          "\033[;93m"      /* Yellow      */
    #define LAL_LOG_COLOR_ERROR            "\033[;91m"      /* Red         */
    #define LAL_LOG_COLOR_FATAL            "\033[;31m"      /* Bright red  */
    #define LAL_LOG_COLOR_END              "\033[0m"        /* Reset color */

#else /* LAL_PLATFORM_SUPPORTS_ANSI_ESCAPES */

    #define LAL_LOG_COLOR_TRACE            ""
    #define LAL_LOG_COLOR_VERBOSE          ""
    #define LAL_LOG_COLOR_INFO             ""
    #define LAL_LOG_COLOR_WARNING          ""
    #define LAL_LOG_COLOR_ERROR            ""
    #define LAL_LOG_COLOR_FATAL            ""
    #define LAL_LOG_COLOR_END              ""

#endif /* !LAL_PLATFORM_SUPPORTS_ANSI_ESCAPES */


/*----------------------------------------------------------------------------
    External logging macros.
----------------------------------------------------------------------------*/

//# Constexpr statement that checks if the log category is enabled at compile time.
#define IS_COMPILED_LOG(Category, Verbosity) \
    (((::Lal::ELogVerbosity::Type::Verbosity) < (Category.GetCompileTimeVerbosity())) == false)

//#
//# Create an inline log category inside a header to transitively enable logging for this category
//# in all translation units that include said header.
//#
//# @param Category  A human-friendly name for the category.
//# @param Verbosity The filter level for the category. If a message is below this level, it will not be logged
//#                  even if the category is enabled.
//#
#define DECLARE_INLINE_LOG_CATEGORY(Category, Verbosity) \
    PRIVATE_LAL_LOG_DECLARE_INLINE_LOG_CATEGORY_IMPL(Category, Verbosity)

//#
//# A macro that logs a formatted message if the log category is active at TRACE verbosity level.
//#
//# @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
//# @param Format       Format string literal in the style of std::format.
//#
#if LAL_LOG_ENABLE_TRACE
    #define LOG_TRACE(Category, Format, ...) \
        PRIVATE_LAL_LOG_PRIVATE_LOG(Category, Trace, LAL_LOG_COLOR_TRACE, Format, ##__VA_ARGS__)
#endif /* LAL_LOG_ENABLE_TRACE */

//#
//# A macro that logs a formatted message if the log category is active at VERBOSE verbosity level.
//#
//# @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
//# @param Format       Format string literal in the style of std::format.
//#
#if LAL_LOG_ENABLE_VERBOSE
    #define LOG_VERBOSE(Category, Format, ...) \
        PRIVATE_LAL_LOG_PRIVATE_LOG(Category, Verbose, LAL_LOG_COLOR_VERBOSE, Format, ##__VA_ARGS__)
#endif /* LAL_LOG_ENABLE_VERBOSE */

//#
//# A macro that logs a formatted message if the log category is active at INFO verbosity level.
//#
//# @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
//# @param Format       Format string literal in the style of std::format.
//#
#if LAL_LOG_ENABLE_INFO
    #define LOG_INFO(Category, Format, ...) \
        PRIVATE_LAL_LOG_PRIVATE_LOG(Category, Info, LAL_LOG_COLOR_INFO, Format, ##__VA_ARGS__)
#endif /* LAL_LOG_ENABLE_INFO */

//#
//# A macro that logs a formatted message if the log category is active at WARNING verbosity level.
//#
//# @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
//# @param Format       Format string literal in the style of std::format.
//#
#if LAL_LOG_ENABLE_WARNING
    #define LOG_WARNING(Category, Format, ...) \
        PRIVATE_LAL_LOG_PRIVATE_LOG(Category, Warning, LAL_LOG_COLOR_WARNING, Format, ##__VA_ARGS__)
#endif /* LAL_LOG_ENABLE_WARNING */

//#
//# A macro that logs a formatted message if the log category is active at ERROR verbosity level.
//#
//# @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
//# @param Format       Format string literal in the style of std::format.
//#
#if LAL_LOG_ENABLE_ERROR
    #define LOG_ERROR(Category, Format, ...) \
        PRIVATE_LAL_LOG_PRIVATE_LOG(Category, Error, LAL_LOG_COLOR_ERROR, Format, ##__VA_ARGS__)
#endif /* LAL_LOG_ENABLE_ERROR */

//#
//# A macro that logs a formatted message if the log category is active at FATAL verbosity level.
//#
//# @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
//# @param Format       Format string literal in the style of std::format.
//#
#define LOG_FATAL(Category, Format, ...)      \
    LAL_GORGEOUS_TRAP_MSG(::LString::SprintF( \
        "[{}] - {}: " Format "", Category.GetCategory(), std::string_view{__FUNCTION__}, ##__VA_ARGS__).ToPtr())
#define PRIVATE_LAL_LOG_FATAL_CORE(Category, Format, ...) \
    LAL_GORGEOUS_TRAP_MSG(std::vformat(                   \
        "[{}] - {}: " Format "", std::make_format_args(Category.GetCategory(), __FUNCTION__, ##__VA_ARGS__)).c_str())


/*----------------------------------------------------------------------------
    Internal and private logging macros.
----------------------------------------------------------------------------*/

#if (LAL_WITH_GCC || LAL_WITH_CLANG)
    #define LAL_FUNCTION_SIG                                            __PRETTY_FUNCTION__
    #define LAL_REAL_FUNC_SIG                                           JAFG_FUNCTION_SIG
#elif LAL_WITH_MSVC
    #define LAL_FUNCTION_SIG                                            __FUNCSIG__
    #define LAL_REAL_FUNC_SIG                                           __FUNCDNAME__
#else /* LAL_WITH_MSVC */
    #error "Compiler missing implementation."
#endif /* !LAL_WITH_MSVC */

//# Pretty function name as [MyNameSpace::MyClass::MyFunction].
#define LAL_PRETTY_FUNCTION                                             (::Lal::PrettyFunctionName(LAL_FUNCTION_SIG))

//# Current class name.
#define PRIVATE_LAL_LOG_TRACE_STR_CUR_CLASS                             (LStringLegacy(__FUNCTION__)

//# Current function name.
#define PRIVATE_LAL_LOG_TRACE_STR_CUR_FUNC                              (LStringLegacy(LAL_REAL_FUNC_SIG))

//# Current function signature.
#define PRIVATE_LAL_LOG_TRACE_STR_CUR_FUNC_SIG                          (LStringLegacy(LAL_FUNCTION_SIG))

//# Current line number.
#define PRIVATE_LAL_LOG_TRACE_STR_CUR_LINE                              (std::to_string(__LINE__))

//# Current class and function name.
#define PRIVATE_LAL_LOG_TRACE_STR_CUR_CLASS_FUNC                        (LStringLegacy(__FUNCTION__))

//# Current class name, function name and line number.
#define LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC_LINE \
    (PRIVATE_JAFG_LOG_TRACE_STR_CUR_FUNC + LITERAL_WIDE([) + PRIVATE_JAFG_LOG_TRACE_STR_CUR_LINE + LITERAL_WIDE(]))

//# Instantly flushes log output. Caller will have to wait.
#define LAL_UNSAFE_FLUSH_OUT_STREAMS()                                  (::Lal::FlushOutStreams());

#define PRIVATE_LAL_LOG_DECLARE_INLINE_LOG_CATEGORY_IMPL(Category, Verbosity) \
    inline ::Lal::LLogCategory<::Lal::ELogVerbosity::Type::Verbosity> Category(#Category);

#define PRIVATE_LAL_LOG_PRIVATE_LOG(Category, Verbosity, Color, Format, ...) \
    ::Lal::LogMessage<::Lal::ELogVerbosity::Verbosity, Category.GetCompileTimeVerbosity()>( \
    Color "[" #Category "] - {}: " Format "" LAL_LOG_COLOR_END, std::string_view{__FUNCTION__}, ##__VA_ARGS__);

#if !LAL_LOG_ENABLE_TRACE
    #define LOG_TRACE(Category, Format, ...)
#endif /* !LAL_LOG_ENABLE_TRACE */

#if !LAL_LOG_ENABLE_VERBOSE
    #define LOG_VERBOSE(Category, Format, ...)
#endif /* !LAL_LOG_ENABLE_VERBOSE */

#if !LAL_LOG_ENABLE_INFO
    #define LOG_INFO(Category, Format, ...)
#endif /* !LAL_LOG_ENABLE_INFO */

#if !LAL_LOG_ENABLE_WARNING
    #define LOG_WARNING(Category, Format, ...)
#endif /* !LAL_LOG_ENABLE_WARNING */

#if !LAL_LOG_ENABLE_ERROR
    #define LOG_ERROR(Category, Format, ...)
#endif /* !LAL_LOG_ENABLE_ERROR */
