// Copyright mzoesch. All rights reserved.

#pragma once

#include <string>
#include <iostream>
#include <cassert>

namespace Jafg::Private
{

/**
 * Defines a log category that can be used to log messages.
 *
 * @param  Category  A human-friendly name for the category.
 * @tparam Verbosity The filter level for the category. If a message is below this level, it will not be logged
 *                   even if the category is enabled.
 */
template <ELogVerbosityType Verbosity>
struct LLogCategory
{
    LLogCategory() = delete;
    PROHIBIT_REALLOC_OF_ANY_FROM(LLogCategory)

    FORCEINLINE explicit LLogCategory(LString Category) : Category(std::move(Category))
    {
        return;
    }

    ~LLogCategory() = default;

    FORCEINLINE static auto GetVerbosity() -> ELogVerbosityType { return Verbosity; }

    FORCEINLINE auto GetCategory() const -> const LString& { return this->Category; }

private:

    LString           Category;
};

/**
 * Log a message to the stdout that may be used with Ansi strings.
 */
FORCEINLINE void LogMessage(const LString&& InAnsiMessage)
{
    std::cout << InAnsiMessage << '\n';
}

/**
 * Log a message to the stdout that may be used with Ansi strings and a message to the platform error
 * output window that may transit the program to a debug state, if attached.
 *
 * @param InMessage     The message to log.
 * @param InAnsiMessage The message to log in Ansi format.
 * @param InFile        The file where the panic occurred.
 * @param InLine        The line where the panic occurred.
 */
FORCEINLINE void LogPanicMessage(const LString&& InMessage, const LString&& InAnsiMessage, const LString&& InFile, const uint32 InLine)
{
    std::cout << InAnsiMessage << '\n';
    /*
     * Should we use cerr? I mean, currently I do not really see a difference between them in the terminal output.
     * But it could get handy if we use python / golang or some other router system for the program.
     * But for now, we do not need to worry about it.
     */
    /* std::cerr << InMessage << '\n'; */

    LOG_PRIVATE_UNSAFE_FLUSH_EVERYTHING_FAST()

#if PLATFORM_WINDOWS

    const std::wstring InMessageWide = std::wstring(InMessage.begin(), InMessage.end());
    const std::wstring InFileWide    = std::wstring(InFile.begin(), InFile.end());

    (void)
    (
        (
            _wassert(InMessageWide.c_str(), InFileWide.c_str(), InLine), 0
        )
    )
    ;

#else /* PLATFORM_WINDOWS */
    #error "Could not resolve PLATFORM."
#endif /* !PLATFORM_WINDOWS */

    return;
}

} /* ~Namespace Jafg::Private */
