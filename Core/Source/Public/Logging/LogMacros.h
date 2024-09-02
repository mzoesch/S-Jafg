// Copyright mzoesch. All rights reserved.

#pragma once


/*----------------------------------------------------------------------------
    Quick switches.
----------------------------------------------------------------------------*/

// #define LOG_WITH_LINE_NUMBERS 0


/*----------------------------------------------------------------------------
    Internal and private logging macros.
----------------------------------------------------------------------------*/

#define LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC \
    (String(__FUNCTION__))

#define LOG_PRIVATE(Category, Verbosity, Format, ...) \
    JafgLog::Private::JafgLogInstance::TryLogMessage(Category::GetCategoryName(), Verbosity, Format, ##__VA_ARGS__);


/*----------------------------------------------------------------------------
    External logging macros.
----------------------------------------------------------------------------*/

#define LOG_TRACE(Category, Format, ...) \
    LOG_PRIVATE(Category, J_TRACE, "\033[;90m{}: " Format "\033[0m", LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC, ##__VA_ARGS__)

#define LOG_DEBUG(Category, Format, ...) \
    LOG_PRIVATE(Category, J_DEBUG, "\033[;37m{}: " Format "\033[0m", LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC, ##__VA_ARGS__)

#define LOG_DISPLAY(Category, Format, ...) \
    LOG_PRIVATE(Category, J_DISPLAY, "\033[;97m{}: " Format "\033[0m", LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC, ##__VA_ARGS__)

#define LOG_WARNING(Category, Format, ...) \
    LOG_PRIVATE(Category, J_WARNING, "\033[;93m{}: " Format "\033[0m", LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC, ##__VA_ARGS__)

#define LOG_ERROR(Category, Format, ...) \
    LOG_PRIVATE(Category, J_ERROR, "\033[;91m{}: " Format "\033[0m", LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC, ##__VA_ARGS__)

#define LOG_FATAL(Category, Format, ...) \
    LOG_PRIVATE(Category, J_FATAL, "\033[;31m{}: " Format "\033[0m", LOG_PRIVATE_TRACE_STR_CUR_CLASS_FUNC, ##__VA_ARGS__)

#define REGISTER_LOG_CATEGORY(Category) \
    JafgLog::Private::JafgLogInstance::TryRegisterLogCategory<Category>();
