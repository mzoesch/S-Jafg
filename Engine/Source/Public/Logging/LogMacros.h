// Copyright mzoesch. All rights reserved.

#pragma once

/*----------------------------------------------------------------------------
    Quick switches.
----------------------------------------------------------------------------*/

//#
//# These macro changes if any logging category can log in said verbosity level.
//#
#ifndef JAFG_LOG_ENABLE_TRACE
    #define JAFG_LOG_ENABLE_TRACE                (IN_DEBUG)
#endif /* JAFG_LOG_ENABLE_TRACE */
#ifndef JAFG_LOG_ENABLE_VERBOSE
    #define JAFG_LOG_ENABLE_VERBOSE              (IN_DEBUG || IN_DEVELOPMENT)
#endif /* JAFG_LOG_ENABLE_VERBOSE */
#ifndef JAFG_LOG_ENABLE_INFO
    #define JAFG_LOG_ENABLE_INFO                 1
#endif /* JAFG_LOG_ENABLE_INFO */
#ifndef JAFG_LOG_ENABLE_WARNING
    #define JAFG_LOG_ENABLE_WARNING              1
#endif /* JAFG_LOG_ENABLE_WARNING */
#ifndef JAFG_LOG_ENABLE_ERROR
    #define JAFG_LOG_ENABLE_ERROR                1
#endif /* JAFG_LOG_ENABLE_ERROR */


/*----------------------------------------------------------------------------
    Logging colors.
----------------------------------------------------------------------------*/

#if JAFG_PLATFORM_SUPPORTS_ANSI_ESCAPES

    #define JAFG_LOG_COLOR_TRACE            "\033[;90m"      /* Dark gray   */
    #define JAFG_LOG_COLOR_VERBOSE          "\033[;37m"      /* Light gray  */
    #define JAFG_LOG_COLOR_INFO             "\033[;97m"      /* White       */
    #define JAFG_LOG_COLOR_WARNING          "\033[;93m"      /* Yellow      */
    #define JAFG_LOG_COLOR_ERROR            "\033[;91m"      /* Red         */
    #define JAFG_LOG_COLOR_FATAL            "\033[;31m"      /* Bright red  */
    #define JAFG_LOG_COLOR_END              "\033[0m"        /* Reset color */

#else /* JAFG_PLATFORM_SUPPORTS_ANSI_ESCAPES */

    #define JAFG_LOG_COLOR_TRACE            ""
    #define JAFG_LOG_COLOR_VERBOSE          ""
    #define JAFG_LOG_COLOR_INFO             ""
    #define JAFG_LOG_COLOR_WARNING          ""
    #define JAFG_LOG_COLOR_ERROR            ""
    #define JAFG_LOG_COLOR_FATAL            ""
    #define JAFG_LOG_COLOR_END              ""

#endif /* !JAFG_PLATFORM_SUPPORTS_ANSI_ESCAPES */


/*----------------------------------------------------------------------------
    External logging macros.
----------------------------------------------------------------------------*/

//# Constexpr statement that checks if the log category is enabled at compile time.
#define IS_COMPILED_LOG(Category, Verbosity) \
    (((::Jafg::ELogVerbosity::Type::Verbosity) < (Category.GetCompileTimeVerbosity())) == false)

//#
//# Create an inline log category inside a header to transitively enable logging for this category
//# in all translation units that include said header.
//#
//# @param Category  A human-friendly name for the category.
//# @param Verbosity The filter level for the category. If a message is below this level, it will not be logged
//#                  even if the category is enabled.
//#
#define DECLARE_INLINE_LOG_CATEGORY(Category, Verbosity) \
    PRIVATE_JAFG_LOG_DECLARE_INLINE_LOG_CATEGORY_IMPL(Category, Verbosity)

//#
//# A macro that logs a formatted message if the log category is active at TRACE verbosity level.
//#
//# @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
//# @param Format       Format string literal in the style of std::format.
//#
#if JAFG_LOG_ENABLE_TRACE
    #define LOG_TRACE(Category, Format, ...) \
        PRIVATE_JAFG_LOG_PRIVATE_LOG(Category, Trace, JAFG_LOG_COLOR_TRACE, Format, ##__VA_ARGS__)
#endif /* JAFG_LOG_ENABLE_TRACE */

//#
//# A macro that logs a formatted message if the log category is active at VERBOSE verbosity level.
//#
//# @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
//# @param Format       Format string literal in the style of std::format.
//#
#if JAFG_LOG_ENABLE_VERBOSE
    #define LOG_VERBOSE(Category, Format, ...) \
        PRIVATE_JAFG_LOG_PRIVATE_LOG(Category, Verbose, JAFG_LOG_COLOR_VERBOSE, Format, ##__VA_ARGS__)
#endif /* JAFG_LOG_ENABLE_VERBOSE */

//#
//# A macro that logs a formatted message if the log category is active at INFO verbosity level.
//#
//# @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
//# @param Format       Format string literal in the style of std::format.
//#
#if JAFG_LOG_ENABLE_INFO
    #define LOG_INFO(Category, Format, ...) \
        PRIVATE_JAFG_LOG_PRIVATE_LOG(Category, Info, JAFG_LOG_COLOR_INFO, Format, ##__VA_ARGS__)
#endif /* JAFG_LOG_ENABLE_INFO */

//#
//# A macro that logs a formatted message if the log category is active at WARNING verbosity level.
//#
//# @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
//# @param Format       Format string literal in the style of std::format.
//#
#if JAFG_LOG_ENABLE_WARNING
    #define LOG_WARNING(Category, Format, ...) \
        PRIVATE_JAFG_LOG_PRIVATE_LOG(Category, Warning, JAFG_LOG_COLOR_WARNING, Format, ##__VA_ARGS__)
#endif /* JAFG_LOG_ENABLE_WARNING */

//#
//# A macro that logs a formatted message if the log category is active at ERROR verbosity level.
//#
//# @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
//# @param Format       Format string literal in the style of std::format.
//#
#if JAFG_LOG_ENABLE_ERROR
    #define LOG_ERROR(Category, Format, ...) \
        PRIVATE_JAFG_LOG_PRIVATE_LOG(Category, Error, JAFG_LOG_COLOR_ERROR, Format, ##__VA_ARGS__)
#endif /* JAFG_LOG_ENABLE_ERROR */

//#
//# A macro that logs a formatted message if the log category is active at FATAL verbosity level.
//#
//# @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
//# @param Format       Format string literal in the style of std::format.
//#
#define LOG_FATAL(Category, Format, ...)  \
    JAFG_GORGEOUS_TRAP_MSG(::Jafg::SprintF( \
        "[{}] - {}: " Format "", Category.GetCategory(), std::string_view{__FUNCTION__}, ##__VA_ARGS__).c_str())
#define PRIVATE_JAFG_LOG_FATAL_CORE(Category, Format, ...) \
    JAFG_GORGEOUS_TRAP_MSG(std::vformat(                   \
        "[{}] - {}: " Format "", std::make_format_args(Category.GetCategory(), __FUNCTION__, ##__VA_ARGS__)).c_str())


/*----------------------------------------------------------------------------
    Internal and private logging macros.
----------------------------------------------------------------------------*/

#if (JAFG_WITH_GCC || JAFG_WITH_CLANG)
    #define JAFG_FUNCTION_SIG                                            __PRETTY_FUNCTION__
    #define JAFG_REAL_FUNC_SIG                                           JAFG_FUNCTION_SIG
#elif JAFG_WITH_MSVC
    #define JAFG_FUNCTION_SIG                                            __FUNCSIG__
    #define JAFG_REAL_FUNC_SIG                                           __FUNCDNAME__
#else /* JAFG_WITH_MSVC */
    #error "Compiler missing implementation."
#endif /* !JAFG_WITH_MSVC */

//# Pretty function name as [MyNameSpace::MyClass::MyFunction].
#define JAFG_PRETTY_FUNCTION                                             (::Jafg::PrettyFunctionName(JAFG_FUNCTION_SIG))

//# Current class name.
#define PRIVATE_JAFG_LOG_TRACE_STR_CUR_CLASS                             (LStringLegacy(__FUNCTION__)

//# Current function name.
#define PRIVATE_JAFG_LOG_TRACE_STR_CUR_FUNC                              (LStringLegacy(JAFG_REAL_FUNC_SIG))

//# Current function signature.
#define PRIVATE_JAFG_LOG_TRACE_STR_CUR_FUNC_SIG                          (LStringLegacy(JAFG_FUNCTION_SIG))

//# Current line number.
#define PRIVATE_JAFG_LOG_TRACE_STR_CUR_LINE                              (std::to_string(__LINE__))

//# Current class and function name.
#define PRIVATE_JAFG_LOG_TRACE_STR_CUR_CLASS_FUNC                        (LStringLegacy(__FUNCTION__))

//# Current class name, function name and line number.
#define LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC_LINE \
    (PRIVATE_JAFG_LOG_TRACE_STR_CUR_FUNC + LITERAL_WIDE([) + PRIVATE_JAFG_LOG_TRACE_STR_CUR_LINE + LITERAL_WIDE(]))

#define PRIVATE_JAFG_LOG_DECLARE_INLINE_LOG_CATEGORY_IMPL(Category, Verbosity) \
    inline ::Jafg::LLogCategory<::Jafg::ELogVerbosity::Type::Verbosity> Category(#Category);

#if JAFG_SAVE_LOGS_IN_MEMORY
    #define PRIVATE_JAFG_LOG_SAVE_LOG(Category, Verbosity, Format, ...)                             \
        ::Jafg::SaveLog<::Jafg::ELogVerbosity::Type::Verbosity, Category.GetCompileTimeVerbosity()>( \
            "[" #Category "] - {}: " Format "", std::string_view{__FUNCTION__}, ##__VA_ARGS__);
#else /* JAFG_SAVE_LOGS_IN_MEMORY */
    #define PRIVATE_JAFG_LOG_SAVE_LOG(Category, Verbosity, Format, ...)
#endif /* !JAFG_SAVE_LOGS_IN_MEMORY */

#define PRIVATE_JAFG_LOG_PRIVATE_LOG(Category, Verbosity, Color, Format, ...)                \
    PRIVATE_JAFG_LOG_SAVE_LOG(Category, Verbosity, Format, ##__VA_ARGS__)                    \
    ::Jafg::LogMessage<::Jafg::ELogVerbosity::Verbosity, Category.GetCompileTimeVerbosity()>( \
        Color "[" #Category "] - {}: " Format "" JAFG_LOG_COLOR_END, std::string_view{__FUNCTION__}, ##__VA_ARGS__);

#if !JAFG_LOG_ENABLE_TRACE
    #define LOG_TRACE(Category, Format, ...)
#endif /* !JAFG_LOG_ENABLE_TRACE */

#if !JAFG_LOG_ENABLE_VERBOSE
    #define LOG_VERBOSE(Category, Format, ...)
#endif /* !JAFG_LOG_ENABLE_VERBOSE */

#if !JAFG_LOG_ENABLE_INFO
    #define LOG_INFO(Category, Format, ...)
#endif /* !JAFG_LOG_ENABLE_INFO */

#if !JAFG_LOG_ENABLE_WARNING
    #define LOG_WARNING(Category, Format, ...)
#endif /* !JAFG_LOG_ENABLE_WARNING */

#if !JAFG_LOG_ENABLE_ERROR
    #define LOG_ERROR(Category, Format, ...)
#endif /* !JAFG_LOG_ENABLE_ERROR */
