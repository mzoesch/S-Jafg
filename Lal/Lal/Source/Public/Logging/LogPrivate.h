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
    PROHIBIT_REALLOC_OF_ANY_FORM(LLogCategory)

    FORCEINLINE explicit LLogCategory(LStringLegacy Category) : Category(std::move(Category))
    {
        return;
    }

    ~LLogCategory() = default;

    FORCEINLINE static constexpr auto GetCompileTimeVerbosity() -> ELogVerbosityType { return Verbosity; }

    FORCEINLINE auto GetCategory() const -> const LStringLegacy& { return this->Category; }

private:

    LStringLegacy           Category;
};

/**
 * Log a message to the stdout that may be used with Ansi strings.
 */
FORCEINLINE void LogMessage(const LStringLegacy&& InAnsiMessage)
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
FORCEINLINE void LogPanicMessage(
    const LStringLegacy&& InMessage,
    const LStringLegacy&& InAnsiMessage,
    const LStringLegacy&& InFile,
    const uint32          InLine
)
{
    std::cout << InAnsiMessage << '\n';
    /*
     * Should we use cerr? I mean, currently I do not really see a difference between them in the terminal output.
     * But it could get handy if we use python / golang or some other router system for the program.
     * But for now, we do not need to worry about it.
     */
    /* std::cerr << InMessage << '\n'; */

    ALWAYS_BREAK_PANIC_WITH_BODY(InMessage, InFile, InLine)

    return;
}

} /* ~Namespace Jafg::Private */
