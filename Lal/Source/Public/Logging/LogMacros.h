// Copyright mzoesch. All rights reserved.

#pragma once

/*----------------------------------------------------------------------------
    Quick switches.
----------------------------------------------------------------------------*/

/**
 * All logs that use the below macros will output logs with exact line numbers.
 * This has no effect on runtime efficiency.
 */
#define LOG_WITH_LINE_NUMBERS       0

/**
 * Disables all logging. This will not affect logs that have the fatal verbosity.
 */
#define DISABLE_ALL_LOGGING         0

/**
 * Will log to a file.
 * @see Logging/LogPrivate.h for file path and name.
 */
#define LOG_TO_FILE                 0


/*----------------------------------------------------------------------------
    Logging colors.
----------------------------------------------------------------------------*/

#define LOG_COLOR_TRACE            "\033[;90m"      /* Dark gray   */
#define LOG_COLOR_DEBUG            "\033[;37m"      /* Light gray  */
#define LOG_COLOR_INFO             "\033[;97m"      /* White       */
#define LOG_COLOR_WARNING          "\033[;93m"      /* Yellow      */
#define LOG_COLOR_ERROR            "\033[;91m"      /* Red         */
#define LOG_COLOR_FATAL            "\033[;31m"      /* Bright red  */

#define LOG_COLOR_END              "\033[0m"        /* Reset color */


/*----------------------------------------------------------------------------
    External logging macros.
----------------------------------------------------------------------------*/

/**
 * Create an inline log category inside a header to transitively enable logging for this category
 * in all translation units that include said header.
 *
 * @param Category  A human-friendly name for the category.
 * @param Verbosity The filter level for the category. If a message is below this level, it will not be logged
 *                  even if the category is enabled.
 */
#define DECLARE_INLINE_LOG_CATEGORY(Category, Verbosity)                   \
    PRIVATE_JAFG_LOG_DECLARE_INLINE_LOG_CATEGORY_IMPL(Category, Verbosity)

/**
 * A macro that logs a formatted message if the log category is active at TRACE verbosity level.
 *
 * @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
 * @param Format       Format string literal in the style of std::format.
 */
#define LOG_TRACE(Category, Format, ...)                                                  \
    PRIVATE_JAFG_LOG_PRIVATE_LOG(Category, Trace, LOG_COLOR_TRACE, Format, ##__VA_ARGS__)

/**
 * A macro that logs a formatted message if the log category is active at DEBUG verbosity level.
 *
 * @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
 * @param Format       Format string literal in the style of std::format.
 */
#define LOG_DEBUG(Category, Format, ...)                                                  \
    PRIVATE_JAFG_LOG_PRIVATE_LOG(Category, Debug, LOG_COLOR_DEBUG, Format, ##__VA_ARGS__)

/**
 * A macro that logs a formatted message if the log category is active at INFO verbosity level.
 *
 * @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
 * @param Format       Format string literal in the style of std::format.
 */
#define LOG_INFO(Category, Format, ...)                                                 \
    PRIVATE_JAFG_LOG_PRIVATE_LOG(Category, Info, LOG_COLOR_INFO, Format, ##__VA_ARGS__)

/**
 * A macro that logs a formatted message if the log category is active at WARNING verbosity level.
 *
 * @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
 * @param Format       Format string literal in the style of std::format.
 */
#define LOG_WARNING(Category, Format, ...)                                                    \
    PRIVATE_JAFG_LOG_PRIVATE_LOG(Category, Warning, LOG_COLOR_WARNING, Format, ##__VA_ARGS__)

/**
 * A macro that logs a formatted message if the log category is active at ERROR verbosity level.
 *
 * @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
 * @param Format       Format string literal in the style of std::format.
 */
#define LOG_ERROR(Category, Format, ...)                                                  \
    PRIVATE_JAFG_LOG_PRIVATE_LOG(Category, Error, LOG_COLOR_ERROR, Format, ##__VA_ARGS__)

/**
 * A macro that logs a formatted message if the log category is active at FATAL verbosity level.
 *
 * @param CategoryName Name of the log category as provided to DECLARE_INLINE_LOG_CATEGORY.
 * @param Format       Format string literal in the style of std::format.
 */
#define LOG_FATAL(Category, Format, ...)                                                        \
    PRIVATE_JAFG_LOG_PRIVATE_PANIC_LOG(Category, Fatal, LOG_COLOR_FATAL, Format, ##__VA_ARGS__)


/*----------------------------------------------------------------------------
    Internal and private logging macros.
----------------------------------------------------------------------------*/

/**
 * Current class name.
 */
#define PRIVATE_JAFG_LOG_TRACE_STR_CUR_CLASS \
    (LString(__FUNCTION__)

/**
 * Current function name.
 */
#define PRIVATE_JAFG_LOG_TRACE_STR_CUR_FUNC \
    (LString(__FUNCDNAME__))

/**
 * Current function signature.
 */
#define PRIVATE_JAFG_LOG_TRACE_STR_CUR_FUNC_SIG \
    (LString(__FUNCSIG__))

/**
 * Current line number.
 */
#define PRIVATE_JAFG_LOG_TRACE_STR_CUR_LINE \
    (std::to_string(__LINE__))

/**
 * Current class and function name.
 */
#define PRIVATE_JAFG_LOG_TRACE_STR_CUR_CLASS_FUNC \
    (LString(__FUNCTION__))

/**
 * Current class name, function name and line number.
 */
#define LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC_LINE                                                                   \
    (PRIVATE_JAFG_LOG_TRACE_STR_CUR_FUNC + LITERAL_WIDE([) + PRIVATE_JAFG_LOG_TRACE_STR_CUR_LINE + LITERAL_WIDE(]))

/** Instantly flushes log output. Caller will have to wait. */
#define LOG_PRIVATE_UNSAFE_FLUSH_EVERYTHING_FAST()                                                 \
    {                      /* This seems weird and it is. But we currently do not really have a */ \
        std::cout.flush(); /* logger lol. We basically just use print statements everywhere but */ \
        std::cerr.flush(); /* hide them behind this complicated macro system to let other       */ \
    }                      /* people think we are professional and somehow ... inhuman?         */

#define PRIVATE_JAFG_LOG_DECLARE_INLINE_LOG_CATEGORY_IMPL(Category, Verbosity)                        \
    inline ::Jafg::Private::LLogCategory<::Jafg::ELogVerbosity::Type::Verbosity> Category(#Category);

#if LOG_WITH_LINE_NUMBERS
    static_assert(false, "LOG_WITH_LINE_NUMBERS is not implemented yet.");
#else /* LOG_WITH_LINE_NUMBERS */
    #define PRIVATE_JAFG_LOG_PRIVATE_LOG(Category, Verbosity, Color, Format, ...)        \
        if (! ( (::Jafg::ELogVerbosity::Type::Verbosity) < (Category.GetVerbosity()) ) ) \
        {                                                                                \
            ::Jafg::Private::LogMessage(                                                 \
                std::format(Color "[{}] - {}: " Format "" LOG_COLOR_END,                 \
                    Category.GetCategory(),                                              \
                    PRIVATE_JAFG_LOG_TRACE_STR_CUR_CLASS_FUNC,                           \
                    ##__VA_ARGS__                                                        \
                )                                                                        \
            );                                                                           \
        }
    #define PRIVATE_JAFG_LOG_PRIVATE_PANIC_LOG(Category, Verbosity, Color, Format, ...)  \
        if (! ( (::Jafg::ELogVerbosity::Type::Verbosity) < (Category.GetVerbosity()) ) ) \
        {                                                                                \
            ::Jafg::Private::LogPanicMessage(                                            \
                std::format("[{}] - {}: " Format "",                                     \
                    Category.GetCategory(),                                              \
                    PRIVATE_JAFG_LOG_TRACE_STR_CUR_CLASS_FUNC,                           \
                    ##__VA_ARGS__                                                        \
                ),                                                                       \
                std::format(Color "[{}] - {}: " Format "" LOG_COLOR_END,                 \
                    Category.GetCategory(),                                              \
                    PRIVATE_JAFG_LOG_TRACE_STR_CUR_CLASS_FUNC,                           \
                    ##__VA_ARGS__                                                        \
                ),                                                                       \
                __FILE__,                                                                \
                __LINE__                                                                 \
            );                                                                           \
        }
#endif /* !LOG_WITH_LINE_NUMBERS */

#if LOG_TO_FILE
    static_assert(false, "LOG_TO_FILE is not implemented yet.");
#endif /* LOG_TO_FILE */
