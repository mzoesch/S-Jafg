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
 */
#if PREPROCESSOR_EXCLUDE_FF
#endif /* PREPROCESSOR_EXCLUDE_FF */

#ifndef PRIVATE_INCLUDED_AUTOMATIC_BUILD
    #error "This file must not be included directly. Include Build/Build.h instead."
#endif /* !PRIVATE_INCLUDED_AUTOMATIC_BUILD_PRIVATE */

#define NO_ENTRY_ASSERT_TEXT        "Enclosing block should never be entered."
#define UNIMPLEMENTED_ASSERT_TEXT   "Unimplemented code path."

#if DO_CHECKS

    /**
     * Allows concatenating multiple statements that are evaporated
     * and therefore not compiled if DO_CHECKS is false.
     */
    #define checkCode(Code)                 do { Code; } while ( false );

    #define check(Expr)                     PRIVATE_JAFG_CHECK_IMPL( Expr )

    #define checkNoEntry()                  PRIVATE_JAFG_CHECK_IMPL( false && NO_ENTRY_ASSERT_TEXT )

    /**
     * Will throw static asserts when compiled IN_SHIPPING. Useful for control paths that are not essential
     * for a quick prototype but should be implemented when encountering them in the wild.
     */
    #define unimplemented()                 PRIVATE_JAFG_CHECK_IMPL( false && UNIMPLEMENTED_ASSERT_TEXT )

    /**
     * Same as check but allows adding a message that cannot be evaluated as a parameter type boolean of the
     * binary operator&&.
     */
    #define checkMsg(Expr, Msg)             PRIVATE_JAFG_CHECK_IMPL_MSG( Expr, Msg )

    /**
     * Same as checkMsg but with a formatted message one in the style of std::format.
     */
    #define checkMsgf(Expr, Format, ...)    PRIVATE_JAFG_CHECK_IMPL_MSGF(Expr, Format, ##__VA_ARGS__)

    // PLATFORM_BREAK() - Currently not using platform break. We of course should be using it.
    // But it is to buggy right now.
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

    #define checkCode(Expr)
    #define check(Expr)
    #define checkNoEntry()
    #define unimplemented()

    #define checkMsg(Expr, Msg)

#endif /* !DO_CHECKS */

#if DO_SLOW_CHECKS

    #define checkSlow(Expr)                 check( Expr )

#else /* DO_SLOW_CHECKS */

    #define checkSlow(expr)

#endif /* !DO_SLOW_CHECKS */

#if DO_ENSURES

    /** !!!Implement this asap. Currently just forwarded.!!! */
    #define ensure(Expr)                    Expr

#else /* !DO_ENSURES */

    #define ensure(Expr)                    Expr

#endif /* !DO_ENSURES */

#if DO_ASSERTS

    #if DO_CHECKS

        #define jassert(Expr)               check( Expr )
        #define jassertNoEntry()            checkNoEntry()
        #define panic(Expr)                 jassert( false && ( Expr ) )

    #else /* DO_CHECKS */

        #define jassert(Expr)               PRIVATE_JAFG_ASSERT_IMPL( Expr )
        #define jassertNoEntry()            PRIVATE_JAFG_ASSERT_IMPL( false && NO_ENTRY_ASSERT_TEXT )
        #define panic(Expr)                 jassert( false && ( Expr ) )

        #define PRIVATE_JAFG_ASSERT_IMPL(Expr)                                   \
            {                                                                    \
                if (UNLIKELY(!(Expr)))                                           \
                {                                                                \
                    LOG_FATAL(LogJafgInternal, "Program panicked: [{}].", #Expr) \
                }                                                                \
            }

    #endif /* !DO_CHECKS */

#else /* DO_ASSERTS */

    #define jassert(Expr)
    #define jassertNoEntry()

    // Panics should allow letting the engine quit immediately.
    // Not even finishing the current tick.
    // But allow for db handlers to be called.
    // It should never corrupt a save file or anything like that.
    // Maybe we want to do a copy of the save file before quitting.
    // Impl tbd.
    #define panic(expr) /* Something here ... */
    #error "Missing panic implementation."

#endif /* !DO_ASSERTS */


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

#if DO_ASSERTS
    #if !DO_CHECKS
        #error "Asserts may not be enabled without checks."
    #endif /* !DO_CHECKS */
#endif /* DO_ASSERTS */
