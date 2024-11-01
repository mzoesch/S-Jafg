// Copyright mzoesch. All rights reserved.

#pragma once

/**
 * Assertion types:
 *   - check:     Compiled if DO_CHECKS is true.
 *                If Expr is false, the program will pause and, if attached, the debugger will be invoked.
 *                A continuing will not be possible.
 *   - checkSlow: Compiled if DO_SLOW_CHECKS is true. Same as checks.
 *   - ensure:    Compiled if DO_ENSURES is true. The Expr will always be evaluated even if DO_ENSURES is false.
 *                If Expr is false, the program will pause and, if attached, the debugger will be invoked.
 *                A continuing will be possible.
 *   - jassert:   Compiled if DO_ASSERTS is true.
 *                If Expr is false, the program will pause and, if attached, the debugger will be invoked.
 *                A continuing will not be possible.
 *   - panic:     Are always compiled.
 *                When encountered, the program will pause and, if attached, the debugger will be invoked.
 *                A continuing will not be possible.
 *
 * Assertion lifetimes:
 *   - DO_CHECKS:      Usually enabled in any development configuration unless overriden in ManualBuildOverride.h.
 *   - DO_SLOW_CHECKS: Usually only enabled in debug configurations unless overriden in ManualBuildOverride.h.
 *   - DO_ENSURES:     Enabled in every configuration where DO_CHECKS is enabled. The same lifetime is always shared.
 *   - DO_ASSERTS:     Usually enabled in any configuration unless overriden in ManualBuildOverride.h.
 *
 * Assertion variants:
 *    - Msg-Suffix:  Allows adding a message that cannot be evaluated as a parameter type boolean of the
 *                   binary operator&&.
 *    - Msgf-Suffix: Allows adding a formatted message in the style of std::format.
 */
#if PREPROCESSOR_EXCLUDE_FF
#endif /* PREPROCESSOR_EXCLUDE_FF */

#ifndef PRIVATE_INCLUDED_AUTOMATIC_BUILD
    #error "This file must not be included directly. Include Build/Build.h instead."
#endif /* !PRIVATE_INCLUDED_AUTOMATIC_BUILD_PRIVATE */

#define NO_ENTRY_ASSERT_TEXT        "Enclosing block should never be entered."
#define UNIMPLEMENTED_ASSERT_TEXT   "Unimplemented code path."
#define FORCED_CODE_PATH_IMPL_TEXT  "Unimplemented code path that was deffered for development "                      \
    "purposes is missing his implementation. Either implement the control path or change serverity of the enclosing " \
    "block by adding a panic or normal check statement."

#if DO_CHECKS

    /**
     * Normal check implementation.
     */
    #define check(Expr)                     PRIVATE_JAFG_CHECK_IMPL( Expr )

    /**
     * Same as check but allows adding a message that cannot be evaluated as a parameter type boolean of the
     * binary operator&&.
     */
    #define checkMsg(Expr, Msg)             PRIVATE_JAFG_CHECK_IMPL_MSG( Expr, Msg )

    /**
     * Same as checkMsg but with a formatted message one in the style of std::format.
     */
    #define checkMsgf(Expr, Format, ...)    PRIVATE_JAFG_CHECK_IMPL_MSGF( Expr, Format, ##__VA_ARGS__ )

    /**
     * Will evaluate to a check that will always fail at runtime.
     */
    #define checkNoEntry()                  PRIVATE_JAFG_CHECK_IMPL( false && NO_ENTRY_ASSERT_TEXT )

    /**
     * Allows concatenating multiple statements that are evaporated
     * and therefore not compiled if DO_CHECKS is false.
     */
    #define checkCode(Code)                 do { Code; } while ( false );

    /**
     * Behaves like a checkNoEntry except when DO_CHECKS is false (usually only IN_SHIPPING), it will
     * evaluate to a static-assert instead of being compiled out like check-like macros. Useful for control paths
     * that are not essential for a quick prototype but should be implemented when encountering them in the wild.
     */
    #define unimplemented()                 PRIVATE_JAFG_CHECK_IMPL( false && UNIMPLEMENTED_ASSERT_TEXT )

    #define PRIVATE_JAFG_CHECK_IMPL(Expr)                                             \
        {                                                                             \
            if (UNLIKELY(!(Expr)))                                                    \
            {                                                                         \
                LOG_FATAL(LogJafgInternal, "Program panicked. Reason: [{}].", #Expr); \
            }                                                                         \
        }

    #define PRIVATE_JAFG_CHECK_IMPL_MSG(Expr, Msg)                                        \
        {                                                                                 \
            if (UNLIKELY(!(Expr)))                                                        \
            {                                                                             \
                LOG_FATAL(LogJafgInternal, "Program panicked: [{}] with {}", #Expr, Msg); \
            }                                                                             \
        }

    #define PRIVATE_JAFG_CHECK_IMPL_MSGF(Expr, Format, ...) \
        {                                                                                                         \
            if (UNLIKELY(!(Expr)))                                                                                \
            {                                                                                                     \
                LOG_FATAL(LogJafgInternal, "Program panicked because of [{}]: " Format "", #Expr, ##__VA_ARGS__); \
            }                                                                                                     \
        }

#else /* DO_CHECKS */

    #define check(Expr)
    #define checkMsg(Expr, Msg)
    #define checkMsgf(Expr, Format, ...)
    #define checkNoEntry()
    #define checkCode(Expr)

#if DO_COMPILER_IGNORE_UNIMPLEMENTED_CTRL_PATHS_IN_SHIPPING
    #define unimplemented()                     panic( "Encounter unimplemented control path forced to be a runtime error." )
#else /* DO_COMPILER_IGNORE_UNIMPLEMENTED_CTRL_PATHS_IN_SHIPPING */
    #define unimplemented()                     static_assert( false, FORCED_CODE_PATH_IMPL_TEXT );
#endif /* !DO_COMPILER_IGNORE_UNIMPLEMENTED_CTRL_PATHS_IN_SHIPPING */

#endif /* !DO_CHECKS */

#if DO_SLOW_CHECKS

    #define checkSlow(Expr)                     check( Expr )
    #define checkSlowMsg(Expr, Msg)             checkMsg( Expr, Msg )
    #define checkSlowMsgf(Expr, Format, ...)    checkMsgf( Expr, Format, ##__VA_ARGS__ )

#else /* DO_SLOW_CHECKS */

    #define checkSlow(expr)
    #define checkSlowMsg(Expr, Msg)
    #define checkSlowMsgf(Expr, Format, ...)

#endif /* !DO_SLOW_CHECKS */

#if DO_ENSURES

    /** !!!Implement this asap. Currently just forwarded.!!! */
    #define ensure(Expr)                    Expr

#else /* !DO_ENSURES */

    #define ensure(Expr)                    Expr

#endif /* !DO_ENSURES */

#if DO_ASSERTS

    #if DO_CHECKS

        #define jassert(Expr)                       check( Expr )
        #define jassertMsg(Expr, Msg)               checkMsg( Expr, Msg )
        #define jassertMsgf(Expr, Format, ...)      checkMsgf( Expr, Format, ##__VA_ARGS__ )

        #define jassertNoEntry()                    checkNoEntry()

        #define panic(Msg)                          jassert( false && ( Msg ) )
        #define panicMsgf(Format, ...)              jassertMsgf( false, Format, ##__VA_ARGS__ )

    #else /* DO_CHECKS */

        #define jassert(Expr)                       PRIVATE_JAFG_ASSERT_IMPL( Expr )
        #define jassertMsg(Expr, Msg)               PRIVATE_JAFG_ASSERT_IMPL_MSG( Expr, Msg )
        #define jassertMsgf(Expr, Format, ...)      PRIVATE_JAFG_ASSERT_IMPL_MSGF( Expr, Format, ##__VA_ARGS__ )

        #define jassertNoEntry()                    PRIVATE_JAFG_ASSERT_IMPL( false && NO_ENTRY_ASSERT_TEXT )

        #define panic(Msg)                          jassert( false && ( Msg ) )
        #define panicMsgf(Format, ...)              jassertMsgf( false, Format, ##__VA_ARGS__ )

        #define PRIVATE_JAFG_ASSERT_IMPL(Expr)                                   \
            {                                                                    \
                if (UNLIKELY(!(Expr)))                                           \
                {                                                                \
                    LOG_FATAL(LogJafgInternal, "Program panicked: [{}].", #Expr) \
                }                                                                \
            }

        #define PRIVATE_JAFG_ASSERT_IMPL_MSG(Expr, Msg)                                       \
            {                                                                                 \
                if (UNLIKELY(!(Expr)))                                                        \
                {                                                                             \
                    LOG_FATAL(LogJafgInternal, "Program panicked: [{}] with {}", #Expr, Msg); \
                }                                                                             \
            }

        #define PRIVATE_JAFG_ASSERT_IMPL_MSGF(Expr, Format, ...)                                                      \
            {                                                                                                         \
                if (UNLIKELY(!(Expr)))                                                                                \
                {                                                                                                     \
                    LOG_FATAL(LogJafgInternal, "Program panicked because of [{}]: " Format "", #Expr, ##__VA_ARGS__); \
                }                                                                                                     \
            }

    #endif /* !DO_CHECKS */

#else /* DO_ASSERTS */

    #define jassert(Expr)
    #define jassertMsg(Expr, Msg)
    #define jassertMsgf(Expr, Format, ...)

    #define jassertNoEntry()

    #define panic(Msg)                    PRIVATE_JAFG_PANIC_IMPL( Msg )
    #define panicMsgf(Format, ...)        PRIVATE_JAFG_PANIC_IMPL_MSGF( Format, ##__VA_ARGS__ )

#define PRIVATE_JAFG_PANIC_IMPL_MSGF(Format, ...)                                                                     \
    {                                                                                                                 \
        LOG_FATAL(LogJafgInternal, "Program panicked encountering unlikely control path: " Format "", ##__VA_ARGS__); \
    }

#endif /* !DO_ASSERTS */


/*-----------------------------------------------------------------------------
    Break macros.
-----------------------------------------------------------------------------*/

/***
 * Will always break the program with a platform-specific error pop-up window.
 * When developing, the program will be able to resume itself.
 */
#if IN_SHIPPING
    #define ALWAYS_BREAK_PANIC(InMessage) \
        PLATFORM_PANIC_BREAK(InMessage)
#else /* IN_SHIPPING */
    #define ALWAYS_BREAK_PANIC(InMessage) \
        PLATFORM_ERROR_BREAK(InMessage)
#endif /* IN_SHIPPING */

/**
 * When not in shipping breaks, the program if a debugger is present else the behavior is undefined.
 */
#if IN_SHIPPING
    #define CONDITIONALLY_BREAK()
#else /* IN_SHIPPING */
    #define CONDITIONALLY_BREAK()       PLATFORM_BREAK()
#endif /* !IN_SHIPPING */


/*-----------------------------------------------------------------------------
    Statically assert defines here to check for illformed configurations.
-----------------------------------------------------------------------------*/

#if !DO_CHECKS
    #if DO_SLOW_CHECKS
        #error "Slow checks may not be enabled without normal checks."
    #endif /* DO_SLOW_CHECKS */
#endif /* !DO_CHECKS */

#if DO_SLOW_CHECKS
    #if !DO_CHECKS
        #error "Slow checks may not be enabled without normal checks."
    #endif /* !DO_CHECKS */
#endif /* DO_SLOW_CHECKS */

#if DO_ENSURES != DO_CHECKS
    #error "Ensures must be enabled if checks are enabled."
#endif /* DO_ENSURES != DO_CHECKS */

#if DO_CHECKS
    #if !DO_ASSERTS
        #error "Checks may not be enabled without asserts."
    #endif /* !DO_ASSERTS */
#endif /* DO_CHECKS */
