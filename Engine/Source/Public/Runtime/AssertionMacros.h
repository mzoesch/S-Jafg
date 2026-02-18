// Copyright mzoesch. All rights reserved.

#pragma once

//#
//# Assertion types:
//#   - check:     Compiled if JAFG_DO_CHECKS is true.
//#                If Expr is false, the program will pause and, if attached, the debugger will be invoked.
//#                A continuing will not be possible.
//#   - checkSlow: Compiled if JAFG_DO_SLOW_CHECKS is true. Same as checks.
//#   - ensure:    Compiled if JAFG_DO_ENSURES is true. If JAFG_DO_ENSURES is false, the expression will be evaluated
//#                but not checked. This is useful if you expect a specific value to be true of a statement that has
//#                side effects and can therefore not be compiled out.
//#                If Expr is false, the program will pause and, if attached, the debugger will be invoked.
//#                A continuing will be possible.
//#   - jassert:   Always compiled.
//#                If Expr is false, the program will pause and, if attached, the debugger will be invoked.
//#                A continuing will not be possible.
//#   - panic:     Always compiled.
//#                When encountered, the program will pause and, if attached, the debugger will be invoked.
//#                A continuing will not be possible.
//#
//# Assertion lifetimes:
//#   - JAFG_DO_CHECKS:      Usually enabled in any development configuration unless overridden manually.
//#   - JAFG_DO_SLOW_CHECKS: Usually only enabled in debug configurations unless overridden manually.
//#   - JAFG_DO_ENSURES:     Enabled in every configuration where JAFG_DO_CHECKS is enabled unless overridden manually.
//#
//# Assertion variants:
//#    - Msg-Suffix:  Allows adding a message that cannot be evaluated as a parameter type boolean of the
//#                   binary operator&&.
//#    - Msgf-Suffix: Allows adding a formatted message in the style of std::format.
//#
#if PREPROCESSOR_EXCLUDE_FF
#endif /* PREPROCESSOR_EXCLUDE_FF */

#ifndef PRIVATE_JAFG_BUILD_H_INCLUDED
    #error "This file must not be included directly. Include Minimal.afx instead."
#endif /* !PRIVATE_JAFG_BUILD_H_INCLUDED */

//#
//# This macro is a wrapper around the #JAFG_PLATFORM_BREAK; that supports a more user-friendly behavior when it comes
//# to debugging or encountering a debug break point without a debugger that is watching us from the distance.
//#
#ifndef JAFG_GORGEOUS_BREAK
    #define JAFG_GORGEOUS_BREAK()                               PRIVATE_JAFG_TRAP_OR_BREAK()
#endif /* JAFG_GORGEOUS_BREAK */
//#
//# Same as #JAFG_GORGEOUS_BREAK but with a message. The message only gets logged if this macro call
//# results in a trap.
//#
#ifndef JAFG_GORGEOUS_BREAK_MSG
    #define JAFG_GORGEOUS_BREAK_MSG(Msg)                        PRIVATE_JAFG_TRAP_OR_BREAK_MSG(Msg)
#endif /* JAFG_GORGEOUS_BREAK_MSG */

//#
//# This macro is a wrapper around the #JAFG_GORGEOUS_TRAP; that supports a more user-friendly behavior when
//# it comes to users for Jafg. It provides a memory-dump and a stack trace to the user in case of a trap.
//#
#ifndef JAFG_GORGEOUS_TRAP
    #define JAFG_GORGEOUS_TRAP()                                PRIVATE_JAFG_GORGEOUS_TRAP_IMPL()
#endif /* JAFG_GORGEOUS_TRAP */
//# Same as #JAFG_GORGEOUS_TRAP but with a custom message. Uhh fancy, right?
#ifndef JAFG_GORGEOUS_TRAP_MSG
    #define JAFG_GORGEOUS_TRAP_MSG(Msg)                         PRIVATE_JAFG_GORGEOUS_TRAP_IMPL_MSG(Msg)
#endif /* JAFG_GORGEOUS_TRAP_MSG */

//#
//# Default check implementation if JAFG_DO_CHECKS is true.
//#
#if JAFG_DO_CHECKS

    //# Normal check implementation.
    #define check(Expr)                         PRIVATE_JAFG_ASSERT_STRONG_IMPL( Expr )

    //#
    //# Same as check but allows adding a message that cannot be evaluated as a parameter type boolean of the
    //# binary operator&&.
    //#
    #define checkMsg(Expr, Msg)                 PRIVATE_JAFG_ASSERT_STRONG_MSG_IMPL( Expr, Msg )

    //# Same as checkMsg but with a formatted message one in the style of std::format.
    #define checkMsgf(Expr, Format, ...)        PRIVATE_JAFG_ASSERT_STRONG_MSGF_IMPL( Expr, Format, __VA_ARGS__ )

    //# Will evaluate to a check that will always fail at runtime.
    #define checkNoEntry()                      PRIVATE_JAFG_ASSERT_STRONG_IMPL_ON_FAIL( JAFG_NO_ENTRY_ASSERT_TEXT )

    //#
    //# Allows concatenating multiple statements that are evaporated
    //# and therefore not compiled if JAFG_DO_CHECKS is false.
    //#
    #define checkCode(Code)                     do { Code; } while ( false );

    //#
    //# Behaves like a checkNoEntry except when JAFG_DO_CHECKS is false (only usually IN_SHIPPING), it will
    //# evaluate to a static-assert instead of being compiled out like check-like macros. Useful for control paths
    //# that are not essential for a quick prototype but should be implemented when encountering them in the wild.
    //#
    #define unimplemented()                     checkNoEntry()

    //#
    //# Do not use std::unreachable but this here as it works directly with the Jafg assertion system.
    //#
    #define unreachable()                       checkNoEntry()

    //#
    //# Macro for constexpr functions that cannot be const-evaluated if checks are enabled.
    //#
    #define CONSTEXPR_CHECK

#endif /* JAFG_DO_CHECKS */

//#
//# Just do the same as checks. If JAFG_DO_SLOW_CHECKS is true, then JAFG_DO_CHECKS must also be true.
//#
#if JAFG_DO_SLOW_CHECKS

    #define checkSlow(Expr)                     check( Expr )
    #define checkSlowMsg(Expr, Msg)             checkMsg( Expr, Msg )
    #define checkSlowMsgf(Expr, Format, ...)    checkMsgf( Expr, Format, __VA_ARGS__ )
    #define checkSlowNoEntry(Expr)              checkNoEntry( Expr )
    #define checkSlowCode(Code)                 checkCode( Code )
    #define CONSTEXPR_CHECK_SLOW

#endif /* JAFG_DO_SLOW_CHECKS */

//#
//# Default ensure implementation if JAFG_DO_ENSURES is true.
//#
#if JAFG_DO_ENSURES

    #define ensure(Expr)                        PRIVATE_JAFG_ENSURE_IMPL(Expr)

    //#
    //# Same as #ensure, but this will cast the result to void, so that the compiler
    //# does not do a tsundere about fucking unused results.
    //#
    #define ensureDiscard(Expr)                 ((void) PRIVATE_JAFG_ENSURE_IMPL(Expr))

#endif /* JAFG_DO_ENSURES */

//#
//# A check implementation that is always compiled.
//# Usually you should only use them if you cannot test the expression at development time.
//# E.g., due to bad user input, network issues, platform access right issues, etc.
//# These failures should be critical, because if not, then you could just create an error pop-up or a log message,
//# and then try to bring the program into a valid state again (because nobody likes crashes in release builds.
//#
#define jassert(Expr)                           PRIVATE_JAFG_ASSERT_STRONG_IMPL( Expr )
#define jassertMsg(Expr, Msg)                   PRIVATE_JAFG_ASSERT_STRONG_MSG_IMPL( Expr, Msg )
#define jassertMsgf(Expr, Format, ...)          PRIVATE_JAFG_ASSERT_STRONG_MSGF_IMPL( Expr, Format, __VA_ARGS__ )
#define jassertNoEntry()                        PRIVATE_JAFG_ASSERT_STRONG_IMPL_ON_FAIL( JAFG_NO_ENTRY_ASSERT_TEXT )

//#
//# Same as jassertNoEntry but more user-friendly with a message. Eww, is this that GoLang?
//#
#define panic(Msg)                              JAFG_GORGEOUS_TRAP_MSG( "Program panicked. " Msg )
#define panicMsgf(Format, ...)                  JAFG_GORGEOUS_TRAP_MSG( ::Jafg::SprintF             \
                                                (                                                 \
                                                    "Program panicked. " Format ""  __VA_OPT__(,) __VA_ARGS__ \
                                                ).c_str() )


/*----------------------------------------------------------------------------
    Implementation if Jafg evaluates check and their different kinds as unnecessary.
----------------------------------------------------------------------------*/

//#
//# Default implementation if JAFG_DO_CHECKS is false. We just remove them.
//#
#if !JAFG_DO_CHECKS

    #define check(Expr)
    #define checkMsg(Expr, Msg)
    #define checkMsgf(Expr, Format, ...)
    #define checkNoEntry()
    #define checkCode(Code)

    #if JAFG_DO_COMPILER_IGNORE_UNIMPLEMENTED_CTRL_PATHS_IN_SHIPPING
        #define unimplemented()                     panic( "Encounter unimplemented control path forced to be a runtime error." )
    #else /* JAFG_DO_COMPILER_IGNORE_UNIMPLEMENTED_CTRL_PATHS_IN_SHIPPING */
        #define unimplemented()                     static_assert( false, JAFG_FORCED_CODE_PATH_IMPL_TEXT );
    #endif /* !JAFG_DO_COMPILER_IGNORE_UNIMPLEMENTED_CTRL_PATHS_IN_SHIPPING */

    #define unreachable()                           JAFG_PLATFORM_UNREACHABLE()

    #define CONSTEXPR_CHECK                         constexpr

#endif /* !JAFG_DO_CHECKS */

//#
//# Default implementation if JAFG_DO_SLOW_CHECKS is false. We just remove them.
//#
#if !JAFG_DO_SLOW_CHECKS

    #define checkSlow(expr)
    #define checkSlowMsg(Expr, Msg)
    #define checkSlowMsgf(Expr, Format, ...)
    #define checkSlowNoEntry(Expr)
    #define checkSlowCode(Code)
    #define CONSTEXPR_CHECK_SLOW                    constexpr

#endif /* !JAFG_DO_SLOW_CHECKS */

//#
//# Default implementation if JAFG_DO_ENSURES is false. We just evaluate the expression but do not check it.
//#
#if !JAFG_DO_ENSURES

    #define ensure(Expr)                        (JAFG_LIKELY(Expr))
    #define ensureDiscard(Expr)                 ((void)(Expr))

#endif /* !JAFG_DO_ENSURES */


/*----------------------------------------------------------------------------
    Private.
----------------------------------------------------------------------------*/

#define JAFG_NO_ENTRY_ASSERT_TEXT        "Enclosing block should never be entered."
#define JAFG_UNIMPLEMENTED_ASSERT_TEXT   "Unimplemented code path."
#define JAFG_FORCED_CODE_PATH_IMPL_TEXT  "Unimplemented code path that was deffered for development "                  \
    "purposes is missing his implementation. Either implement the control path or change serverity of the enclosing " \
    "block by adding a panic or normal check statement."

//# Just break if a debugger is watching us. Do nothing else.
#define PRIVATE_JAFG_TRY_BREAK_NO_FACADE()       \
    if (::Jafg::Hal::IsTracerPidValidVerySlow()) \
    {                                           \
        JAFG_PLATFORM_BREAK()                   \
    }

//#
//# This halts the program always - but with the most friendly developer experience.
//# If there is a debugger watching us, we will break into it, else we trap Jafg.
//#
#define PRIVATE_JAFG_TRAP_OR_BREAK()                 \
    {                                               \
        if (::Jafg::Hal::IsTracerPidValidVerySlow()) \
        {                                           \
            PRIVATE_JAFG_GORGEOUS_BREAK_IMPL();      \
        }                                           \
        else                                        \
        {                                           \
            ::Jafg::FlushOutStreams();               \
            ::Jafg::LOnPlatformBreak::OnProgramPanic \
            (                                       \
                JAFG_NO_ENTRY_ASSERT_TEXT,           \
                __FILE__,                           \
                __LINE__                            \
            );                                      \
        }                                           \
    }

//#
//# This halts the program always - but with the most friendly developer experience.
//# If there is a debugger watching us, we will break into it, else we trap Jafg.
//#
#define PRIVATE_JAFG_TRAP_OR_BREAK_MSG(Msg)                   \
    {                                                        \
        if (::Jafg::Hal::IsTracerPidValidVerySlow())          \
        {                                                    \
            /* Discard the Msg if a debugger is watching. */ \
            PRIVATE_JAFG_GORGEOUS_BREAK_IMPL();               \
        }                                                    \
        else                                                 \
        {                                                    \
            JAFG_GORGEOUS_TRAP_MSG(Msg)                       \
        }                                                    \
    }

#define PRIVATE_JAFG_GORGEOUS_BREAK_IMPL() \
        ::Jafg::FlushOutStreams();    \
        JAFG_PLATFORM_BREAK()

#define PRIVATE_JAFG_GORGEOUS_TRAP_IMPL() \
    PRIVATE_JAFG_GORGEOUS_TRAP_IMPL_MSG   \
    (                                    \
        JAFG_NO_ENTRY_ASSERT_TEXT         \
    )

#define PRIVATE_JAFG_GORGEOUS_TRAP_IMPL_MSG(Msg) \
    ::Jafg::FlushOutStreams();              \
    PRIVATE_JAFG_TRY_BREAK_NO_FACADE();           \
    ::Jafg::LOnPlatformBreak::OnProgramPanic     \
    (                                           \
        Msg,                                    \
        __FILE__,                               \
        __LINE__                                \
    );

//# Get an expression as a string.
#define PRIVATE_JAFG_ASSERT_STRONG_LOG_EXPR_GET_MSG(Expr) \
        "Program panicked. Reason: [" #Expr "]."

//# Combines an expression with a message.
#define PRIVATE_JAFG_ASSERT_STRONG_LOG_EXPR_MSG_GET_MSG(Expr, Msg) \
    "Program panicked. Reason: [" #Expr "]" Msg

//# Combines an expression with a formatted message.
#define PRIVATE_JAFG_ASSERT_STRONG_LOG_EXPR_MSGF_GET_MSG(Expr, Format, ...) \
    ::Jafg::SprintF                                                         \
    (                                                                      \
        "Program panicked. Reason: [" #Expr "]. " Format "" __VA_OPT__(,) __VA_ARGS__ \
    )

//# Get an expression as a string with ANSI format if the platform supports it.
#define PRIVATE_JAFG_ASSERT_STRONG_LOG_EXPR_GET_MSG_ANSI(Expr) \
    JAFG_LOG_COLOR_FATAL                                       \
    "Program panicked. Reason: [" #Expr "]."                  \
    JAFG_LOG_COLOR_END
#define PRIVATE_JAFG_ASSERT_STRONG_LOG_EXPR_MSG_GET_MSG_ANSI(Expr, Msg) \
    JAFG_LOG_COLOR_FATAL                                                \
    "Program panicked. Reason: [" #Expr "]. " Msg ""                   \
    JAFG_LOG_COLOR_END
#define PRIVATE_JAFG_ASSERT_STRONG_LOG_EXPR_MSGF_GET_MSG_ANSI(Expr, Format, ...) \
    ::Jafg::SprintF                                                              \
    (                                                                           \
        JAFG_LOG_COLOR_FATAL                                                     \
        "Program panicked. Reason: [" #Expr "]. " Format "" __VA_OPT__(,) __VA_ARGS__      \
        JAFG_LOG_COLOR_END                                                       \
    )

//# Get an expression as a string.
#define PRIVATE_JAFG_ASSERT_WEAK__LOG_EXPR_GET_MSG(Expr) \
    "Program run into an error. Reason: [" #Expr "]."

//# Combines an expression with a message.
#define PRIVATE_JAFG_ASSERT_WEAK_LOG_EXPR_MSG_GET_MSG(Expr, Msg) \
    "Program run into an error. Reason: [" #Expr "]" Msg

//# Combines an expression with a formatted message.
#define PRIVATE_JAFG_ASSERT_WEAK_LOG_EXPR_MSGF_GET_MSG(Expr, Format, ...)            \
    ::Jafg::SprintF                                                                  \
    (                                                                               \
        "Program run into an error. Reason: [" #Expr "]. " Format "" __VA_OPT__(,) __VA_ARGS__ \
    )

//# Get an expression as a string with ANSI format if the platform supports it.
#define PRIVATE_JAFG_ASSERT_WEAK_LOG_EXPR_GET_MSG_ANSI(Expr) \
    JAFG_LOG_COLOR_ERROR                                     \
    "Program run into an error. Reason: [" #Expr "]."       \
    JAFG_LOG_COLOR_END
#define PRIVATE_JAFG_ASSERT_WEAK_LOG_EXPR_MSG_GET_MSG_ANSI(Expr, Msg) \
    JAFG_LOG_COLOR_ERROR                                              \
    "Program run into an error. Reason: [" #Expr "]. " Msg ""        \
    JAFG_LOG_COLOR_END
#define PRIVATE_JAFG_ASSERT_WEAK_LOG_EXPR_MSGF_GET_MSG_ANSI(Expr, Format, ...)       \
    ::Jafg::SprintF                                                                  \
    (                                                                               \
        JAFG_LOG_COLOR_ERROR                                                         \
        "Program run into an error. Reason: [" #Expr "]. " Format "" __VA_OPT__(,) __VA_ARGS__ \
        JAFG_LOG_COLOR_END                                                           \
    )

//# Log an expression that failed to assert.
#define PRIVATE_JAFG_ASSERT_STRONG_LOG_EXPR(Expr)              \
    ::Jafg::LogMessage<                                        \
        ::Jafg::ELogVerbosity::Fatal,                          \
        ::Jafg::ELogVerbosity::Fatal                           \
    >                                                         \
    (                                                         \
        PRIVATE_JAFG_ASSERT_STRONG_LOG_EXPR_GET_MSG_ANSI(Expr) \
    );

//# Log an expression that failed to assert but is not critical.
#define PRIVATE_JAFG_ASSERT_WEAK_LOG_EXPR(Expr)              \
    ::Jafg::LogMessage<                                      \
        ::Jafg::ELogVerbosity::Error,                        \
        ::Jafg::ELogVerbosity::Error                         \
    >                                                       \
    (                                                       \
        PRIVATE_JAFG_ASSERT_WEAK_LOG_EXPR_GET_MSG_ANSI(Expr) \
    );

//# Assert with a OnFail delegate.
#define PRIVATE_JAFG_ASSERT_IMPL(Expr, OnFail) \
    if (JAFG_UNLIKELY(!(Expr)))                \
    {                                         \
        OnFail                                \
    }                                         \

//# Allows continuing.
#define PRIVATE_JAFG_ASSERT_WEAK_IMPL(Expr)         \
    PRIVATE_JAFG_ASSERT_IMPL                        \
    (                                              \
        Expr,                                      \
        PRIVATE_JAFG_ASSERT_WEAK_IMPL_ON_FAIL(Expr) \
    )

#define PRIVATE_JAFG_ASSERT_WEAK_IMPL_ON_FAIL(Expr)  \
    PRIVATE_JAFG_ASSERT_WEAK_LOG_EXPR(Expr)          \
    {                                               \
        if (::Jafg::Hal::IsTracerPidValidVerySlow()) \
        {                                           \
            PRIVATE_JAFG_GORGEOUS_BREAK_IMPL()       \
        }                                           \
    }

//# Disallows continuing.
#define PRIVATE_JAFG_ASSERT_STRONG_IMPL(Expr)         \
    PRIVATE_JAFG_ASSERT_IMPL                          \
    (                                                \
        Expr,                                        \
        PRIVATE_JAFG_ASSERT_STRONG_IMPL_ON_FAIL(Expr) \
    )

#define PRIVATE_JAFG_ASSERT_STRONG_IMPL_ON_FAIL(Expr) \
    JAFG_GORGEOUS_TRAP_MSG(PRIVATE_JAFG_ASSERT_STRONG_LOG_EXPR_GET_MSG(Expr))

//# Disallows continuing.
#define PRIVATE_JAFG_ASSERT_STRONG_MSG_IMPL(Expr, Msg)         \
    PRIVATE_JAFG_ASSERT_IMPL                                   \
    (                                                         \
        Expr,                                                 \
        PRIVATE_JAFG_ASSERT_STRONG_MSG_IMPL_ON_FAIL(Expr, Msg) \
    )

#define PRIVATE_JAFG_ASSERT_STRONG_MSG_IMPL_ON_FAIL(Expr, Msg)                        \
    JAFG_GORGEOUS_TRAP_MSG(PRIVATE_JAFG_ASSERT_STRONG_LOG_EXPR_MSG_GET_MSG(Expr, Msg)) \

//# Disallows continuing.
#define PRIVATE_JAFG_ASSERT_STRONG_MSGF_IMPL(Expr, Format, ...)                   \
    PRIVATE_JAFG_ASSERT_IMPL                                                      \
    (                                                                            \
        Expr,                                                                    \
        PRIVATE_JAFG_ASSERT_STRONG_MSGF_IMPL_ON_FAIL(Expr, Format, __VA_ARGS__) \
    )

#define PRIVATE_JAFG_ASSERT_STRONG_MSGF_IMPL_ON_FAIL(Expr, Format, ...)                                          \
    JAFG_GORGEOUS_TRAP_MSG(PRIVATE_JAFG_ASSERT_STRONG_LOG_EXPR_MSGF_GET_MSG(Expr, Format, __VA_ARGS__).c_str()) \

#define PRIVATE_JAFG_ENSURE_IMPL(Expr)              \
    (JAFG_LIKELY(Expr) || [](void) -> bool          \
    {                                              \
        PRIVATE_JAFG_ASSERT_WEAK_IMPL_ON_FAIL(Expr) \
        return false;                              \
    }())


/*-----------------------------------------------------------------------------
    Statically assert defines here to check for malformed configurations.
-----------------------------------------------------------------------------*/

#if !JAFG_DO_CHECKS
    #if JAFG_DO_SLOW_CHECKS
        #error "Slow checks may not be enabled without normal checks."
    #endif /* JAFG_DO_SLOW_CHECKS */
#endif /* !JAFG_DO_CHECKS */

#if JAFG_DO_SLOW_CHECKS
    #if !JAFG_DO_CHECKS
        #error "Slow checks may not be enabled without normal checks."
    #endif /* !JAFG_DO_CHECKS */
#endif /* JAFG_DO_SLOW_CHECKS */

#if JAFG_DO_ENSURES != JAFG_DO_CHECKS
    #error "Ensures must be enabled if checks are enabled."
#endif /* JAFG_DO_ENSURES != JAFG_DO_CHECKS */
