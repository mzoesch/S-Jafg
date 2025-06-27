// Copyright mzoesch. All rights reserved.

#pragma once

//#
//# Assertion types:
//#   - check:     Compiled if LAL_DO_CHECKS is true.
//#                If Expr is false, the program will pause and, if attached, the debugger will be invoked.
//#                A continuing will not be possible.
//#   - checkSlow: Compiled if LAL_DO_SLOW_CHECKS is true. Same as checks.
//#   - ensure:    Compiled if LAL_DO_ENSURES is true. If LAL_DO_ENSURES is false, the expression will be evaluated
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
//#   - LAL_DO_CHECKS:      Usually enabled in any development configuration unless overridden manually.
//#   - LAL_DO_SLOW_CHECKS: Usually only enabled in debug configurations unless overridden manually.
//#   - LAL_DO_ENSURES:     Enabled in every configuration where LAL_DO_CHECKS is enabled unless overridden manually.
//#
//# Assertion variants:
//#    - Msg-Suffix:  Allows adding a message that cannot be evaluated as a parameter type boolean of the
//#                   binary operator&&.
//#    - Msgf-Suffix: Allows adding a formatted message in the style of std::format.
//#
#if PREPROCESSOR_EXCLUDE_FF
#endif /* PREPROCESSOR_EXCLUDE_FF */

#ifndef PRIVATE_LAL_BUILD_H_INCLUDED
    #error "This file must not be included directly. Include CoreAfx.h instead."
#endif /* !PRIVATE_LAL_BUILD_H_INCLUDED */

//#
//# This macro is a wrapper around the #LAL_PLATFORM_BREAK; that supports a more user-friendly behavior when it comes
//# to debugging or encountering a debug break point without a debugger that is watching us from the distance.
//#
#ifndef LAL_PLATFORM_GORGEOUS_BREAK
    #define LAL_PLATFORM_GORGEOUS_BREAK()                               PRIVATE_LAL_PLATFORM_TRAP_OR_BREAK()
#endif /* LAL_PLATFORM_GORGEOUS_BREAK */
//#
//# Same as #LAL_PLATFORM_GORGEOUS_BREAK but with a message. The message only gets logged if this macro call
//# results in a trap.
//#
#ifndef LAL_PLATFORM_GORGEOUS_BREAK_MSG
    #define LAL_PLATFORM_GORGEOUS_BREAK_MSG(Msg)                        PRIVATE_LAL_PLATFORM_TRAP_OR_BREAK_MSG(Msg)
#endif /* LAL_PLATFORM_GORGEOUS_BREAK_MSG */

//#
//# This macro is a wrapper around the #LAL_PLATFORM_GORGEOUS_TRAP; that supports a more user-friendly behavior when
//# it comes to users for Jafg. It provides a memory-dump and a stack trace to the user in case of a trap.
//#
#ifndef LAL_PLATFORM_GORGEOUS_TRAP
    #define LAL_PLATFORM_GORGEOUS_TRAP()                                PRIVATE_LAL_PLATFORM_GORGEOUS_TRAP_IMPL()
#endif /* LAL_PLATFORM_GORGEOUS_TRAP */
//# Same as #LAL_PLATFORM_GORGEOUS_TRAP but with a custom message. Uhh fancy, right?
#ifndef LAL_PLATFORM_GORGEOUS_TRAP_MSG
    #define LAL_PLATFORM_GORGEOUS_TRAP_MSG(Msg)                         PRIVATE_LAL_PLATFORM_GORGEOUS_TRAP_IMPL_MSG(Msg)
#endif /* LAL_PLATFORM_GORGEOUS_TRAP_MSG */

//#
//# Default check implementation if LAL_DO_CHECKS is true.
//#
#if LAL_DO_CHECKS

    //# Normal check implementation.
    #define check(Expr)                         PRIVATE_LAL_ASSERT_STRONG_IMPL( Expr )

    //#
    //# Same as check but allows adding a message that cannot be evaluated as a parameter type boolean of the
    //# binary operator&&.
    //#
    #define checkMsg(Expr, Msg)                 PRIVATE_LAL_ASSERT_STRONG_MSG_IMPL( Expr, Msg )

    //# Same as checkMsg but with a formatted message one in the style of std::format.
    #define checkMsgf(Expr, Format, ...)        PRIVATE_LAL_ASSERT_STRONG_MSGF_IMPL( Expr, Format, ##__VA_ARGS__ )

    //# Will evaluate to a check that will always fail at runtime.
    #define checkNoEntry()                      PRIVATE_LAL_ASSERT_STRONG_IMPL_ON_FAIL( LAL_NO_ENTRY_ASSERT_TEXT )

    //#
    //# Allows concatenating multiple statements that are evaporated
    //# and therefore not compiled if LAL_DO_CHECKS is false.
    //#
    #define checkCode(Code)                     do { Code; } while ( false );

    //#
    //# Behaves like a checkNoEntry except when LAL_DO_CHECKS is false (only usually IN_SHIPPING), it will
    //# evaluate to a static-assert instead of being compiled out like check-like macros. Useful for control paths
    //# that are not essential for a quick prototype but should be implemented when encountering them in the wild.
    //#
    #define unimplemented()                     checkNoEntry()

#endif /* LAL_DO_CHECKS */

//#
//# Just do the same as checks. If LAL_DO_SLOW_CHECKS is true, then LAL_DO_CHECKS must also be true.
//#
#if LAL_DO_SLOW_CHECKS

    #define checkSlow(Expr)                     check( Expr )
    #define checkSlowMsg(Expr, Msg)             checkMsg( Expr, Msg )
    #define checkSlowMsgf(Expr, Format, ...)    checkMsgf( Expr, Format, ##__VA_ARGS__ )
    #define checkSlowNoEntry(Expr)              checkNoEntry( Expr )
    #define checkSlowCode(Code)                 checkCode( Code )

#endif /* LAL_DO_SLOW_CHECKS */

//#
//# Default ensure implementation if LAL_DO_ENSURES is true.
//#
#if LAL_DO_ENSURES

    /** !!!Implement this asap. Currently just forwarded.!!! */
    #define ensure(Expr)                    Expr

#endif /* LAL_DO_ENSURES */

//#
//# A check implementation that is always compiled.
//# Usually you should only use them if you cannot test the expression at development time.
//# E.g., due to bad user input, network issues, platform access right issues, etc.
//# These failures should be critical, because if not, then you could just create an error pop-up or a log message,
//# and then try to bring the program into a valid state again (because nobody likes crashes in release builds.
//#
#define jassert(Expr)                           PRIVATE_LAL_ASSERT_STRONG_IMPL( Expr )
#define jassertMsg(Expr, Msg)                   PRIVATE_LAL_ASSERT_STRONG_MSG_IMPL( Expr, Msg )
#define jassertMsgf(Expr, Format, ...)          PRIVATE_LAL_ASSERT_STRONG_MSGF_IMPL( Expr, Format, ##__VA_ARGS__ )
#define jassertNoEntry()                        PRIVATE_LAL_ASSERT_STRONG_IMPL_ON_FAIL( LAL_NO_ENTRY_ASSERT_TEXT )

//#
//# Same as jassertNoEntry but more user-friendly with a message.
//#
#define panic(Msg)                              LAL_PLATFORM_GORGEOUS_TRAP_MSG( "Program panicked. " Msg )
#define panicMsgf(Format, ...)                  LAL_PLATFORM_GORGEOUS_TRAP_MSG( ::Jafg::LString::SprintF\
                                                ( \
                                                    "Program panicked. " Format "",\
                                                    ##__VA_ARGS__                                     \
                                                ).ToPtr() )





/*----------------------------------------------------------------------------
    Implementation if Lal evaluates check and their different kinds as unnecessary.
----------------------------------------------------------------------------*/

//#
//# Default implementation if LAL_DO_CHECKS is false. We just remove them.
//#
#if !LAL_DO_CHECKS

    #define check(Expr)
    #define checkMsg(Expr, Msg)
    #define checkMsgf(Expr, Format, ...)
    #define checkNoEntry()
    #define checkCode(Code)

    #if LAL_DO_COMPILER_IGNORE_UNIMPLEMENTED_CTRL_PATHS_IN_SHIPPING
        #define unimplemented()                     panic( "Encounter unimplemented control path forced to be a runtime error." )
    #else /* LAL_DO_COMPILER_IGNORE_UNIMPLEMENTED_CTRL_PATHS_IN_SHIPPING */
        #define unimplemented()                     static_assert( false, LAL_FORCED_CODE_PATH_IMPL_TEXT );
    #endif /* !LAL_DO_COMPILER_IGNORE_UNIMPLEMENTED_CTRL_PATHS_IN_SHIPPING */

#endif /* !LAL_DO_CHECKS */

//#
//# Default implementation if LAL_DO_SLOW_CHECKS is false. We just remove them.
//#
#if !LAL_DO_SLOW_CHECKS

    #define checkSlow(expr)
    #define checkSlowMsg(Expr, Msg)
    #define checkSlowMsgf(Expr, Format, ...)
    #define checkSlowNoEntry(Expr)
    #define checkSlowCode(Code)

#endif /* !LAL_DO_SLOW_CHECKS */

//#
//# Default implementation if LAL_DO_ENSURES is false. We just evaluate the expression but do not check it.
//#
#if !LAL_DO_ENSURES

    #define ensure(Expr)                        Expr

#endif /* !LAL_DO_ENSURES */


/*----------------------------------------------------------------------------
    Private.
----------------------------------------------------------------------------*/

#define LAL_NO_ENTRY_ASSERT_TEXT        "Enclosing block should never be entered."
#define LAL_UNIMPLEMENTED_ASSERT_TEXT   "Unimplemented code path."
#define LAL_FORCED_CODE_PATH_IMPL_TEXT  "Unimplemented code path that was deffered for development "                  \
    "purposes is missing his implementation. Either implement the control path or change serverity of the enclosing " \
    "block by adding a panic or normal check statement."

//# Just break if a debugger is watching us. Do nothing else.
#define PRIVATE_LAL_TRY_BREAK_NO_FACADE()       \
    if (::Lal::Hal::IsTracerPidValidVerySlow()) \
    {                                           \
        LAL_PLATFORM_BREAK();                   \
    }

#define PRIVATE_LAL_PLATFORM_TRAP_OR_BREAK()            \
    {                                                   \
        if (::Lal::Hal::IsTracerPidValidVerySlow())     \
        {                                               \
            PRIVATE_LAL_PLATFORM_GORGEOUS_BREAK_IMPL(); \
        }                                               \
        else                                            \
        {                                               \
            LAL_PLATFORM_GORGEOUS_TRAP()                \
        }                                               \
    }

#define PRIVATE_LAL_PLATFORM_TRAP_OR_BREAK_MSG(Msg)          \
    {                                                        \
        if (::Lal::Hal::IsTracerPidValidVerySlow())          \
        {                                                    \
            /* Discard the Msg if a debugger is watching. */ \
            PRIVATE_LAL_PLATFORM_GORGEOUS_BREAK_IMPL();      \
        }                                                    \
        else                                                 \
        {                                                    \
            LAL_PLATFORM_GORGEOUS_TRAP_MSG(Msg)              \
        }                                                    \
    }

#define PRIVATE_LAL_PLATFORM_GORGEOUS_BREAK_IMPL() \
    {                                              \
        LAL_UNSAFE_FLUSH_OUT_STREAMS()             \
        LAL_PLATFORM_BREAK()                       \
    }

#define PRIVATE_LAL_PLATFORM_GORGEOUS_TRAP_IMPL() \
    PRIVATE_LAL_PLATFORM_GORGEOUS_TRAP_IMPL_MSG   \
    (                                             \
        LAL_NO_ENTRY_ASSERT_TEXT                  \
    )

#define PRIVATE_LAL_PLATFORM_GORGEOUS_TRAP_IMPL_MSG(Msg)  \
    LAL_UNSAFE_FLUSH_OUT_STREAMS()                        \
    PRIVATE_LAL_TRY_BREAK_NO_FACADE()                     \
    ::Lal::LOnPlatformBreak::OnProgramPanic               \
    (                                                     \
        Msg,                                              \
        __FILE__,                                         \
        __LINE__                                          \
    );

//# Get an expression as a string.
#define PRIVATE_LAL_ASSERT_LOG_EXPR_GET_MSG(Expr)     \
        "Program panicked. Reason: [" #Expr "]."

//# Combines an expression with a message.
#define PRIVATE_LAL_ASSERT_LOG_EXPR_MSG_GET_MSG(Expr, Msg)     \
        "Program panicked. Reason: [" #Expr "]" Msg

//# Combines an expression with a message.
#define PRIVATE_LAL_ASSERT_LOG_EXPR_MSGF_GET_MSG(Expr, Format, ...)     \
    ::Jafg::LString::SprintF\
    ( \
        "Program panicked. Reason: [" #Expr "]. " Format "", ##__VA_ARGS__                                     \
    )

//# Get an expression as a string with ANSI format if the platform supports it.
#if LAL_PLATFORM_SUPPORTS_ANSI_ESCAPES
    #define PRIVATE_LAL_ASSERT_LOG_EXPR_GET_MSG_ANSI(Expr)     \
            LOG_COLOR_FATAL \
            "Program panicked. Reason: [" #Expr "]."\
            LOG_COLOR_END
    #define PRIVATE_LAL_ASSERT_LOG_EXPR_MSG_GET_MSG_ANSI(Expr, Msg)     \
            LOG_COLOR_FATAL \
            "Program panicked. Reason: [" #Expr "]. " Msg ""\
            LOG_COLOR_END
    #define PRIVATE_LAL_ASSERT_LOG_EXPR_MSGF_GET_MSG_ANSI(Expr, Format, ...)     \
        ::Jafg::LString::SprintF\
        ( \
            LOG_COLOR_FATAL \
            "Program panicked. Reason: [" #Expr "]. " Format "", ##__VA_ARGS__                                     \
            LOG_COLOR_END \
        )
#else /* LAL_PLATFORM_SUPPORTS_ANSI_ESCAPES */
    #define PRIVATE_LAL_ASSERT_LOG_EXPR_GET_MSG_ANSI(Expr) PRIVATE_LAL_ASSERT_LOG_EXPR_GET_MSG(Expr)
    #define PRIVATE_LAL_ASSERT_LOG_EXPR_MSG_GET_MSG_ANSI(Expr, Msg) PRIVATE_LAL_ASSERT_LOG_EXPR_MSG_GET_MSG(Expr, Msg)
    #deinfe PRIVATE_LAL_ASSERT_LOG_EXPR_MSGF_GET_MSG_ANSI(Expr, Format, ...) \
        PRIVATE_LAL_ASSERT_LOG_EXPR_MSGF_GET_MSG(Expr, Format, ##__VA_ARGS__)
#endif /* !LAL_PLATFORM_SUPPORTS_ANSI_ESCAPES */

//# Log an expression that failed to assert.
#define PRIVATE_LAL_ASSERT_LOG_EXPR(Expr)     \
    ::Lal::LogMessage                         \
    (                                         \
        PRIVATE_LAL_ASSERT_LOG_EXPR_GET_MSG_ANSI(Expr) \
    );

//# Assert with a OnFail delegate.
#define PRIVATE_LAL_ASSERT_IMPL(Expr, OnFail) \
    if (LAL_UNLIKELY(!(Expr)))                \
    {                                         \
        OnFail                                \
    }                                         \

//# Allows continuing.
#define PRIVATE_LAL_ASSERT_WEAK_IMPL(Expr)         \
    PRIVATE_LAL_ASSERT_IMPL                        \
    (                                              \
        Expr,                                      \
        PRIVATE_LAL_ASSERT_WEAK_IMPL_ON_FAIL(Expr) \
    )

#define PRIVATE_LAL_ASSERT_WEAK_IMPL_ON_FAIL(Expr) \
    /*PRIVATE_LAL_ASSERT_LOG_EXPR(Expr) */             \
    LAL_PLATFORM_GORGEOUS_BREAK()

//# Disallows continuing.
#define PRIVATE_LAL_ASSERT_STRONG_IMPL(Expr)         \
    PRIVATE_LAL_ASSERT_IMPL                          \
    (                                                \
        Expr,                                        \
        PRIVATE_LAL_ASSERT_STRONG_IMPL_ON_FAIL(Expr) \
    )

#define PRIVATE_LAL_ASSERT_STRONG_IMPL_ON_FAIL(Expr) \
    /*PRIVATE_LAL_ASSERT_LOG_EXPR(Expr) */               \
    LAL_PLATFORM_GORGEOUS_TRAP_MSG(PRIVATE_LAL_ASSERT_LOG_EXPR_GET_MSG(Expr)) \

//# Disallows continuing.
#define PRIVATE_LAL_ASSERT_STRONG_MSG_IMPL(Expr, Msg)         \
    PRIVATE_LAL_ASSERT_IMPL                          \
    (                                                \
        Expr,                                        \
        PRIVATE_LAL_ASSERT_STRONG_MSG_IMPL_ON_FAIL(Expr, Msg) \
    )

#define PRIVATE_LAL_ASSERT_STRONG_MSG_IMPL_ON_FAIL(Expr, Msg) \
    LAL_PLATFORM_GORGEOUS_TRAP_MSG(PRIVATE_LAL_ASSERT_LOG_EXPR_MSG_GET_MSG(Expr, Msg)) \

//# Disallows continuing.
#define PRIVATE_LAL_ASSERT_STRONG_MSGF_IMPL(Expr, Format, ...)         \
    PRIVATE_LAL_ASSERT_IMPL                          \
    (                                                \
        Expr,                                        \
        PRIVATE_LAL_ASSERT_STRONG_MSGF_IMPL_ON_FAIL(Expr, Format, ##__VA_ARGS__) \
    )

#define PRIVATE_LAL_ASSERT_STRONG_MSGF_IMPL_ON_FAIL(Expr, Format, ...) \
    LAL_PLATFORM_GORGEOUS_TRAP_MSG(PRIVATE_LAL_ASSERT_LOG_EXPR_MSGF_GET_MSG(Expr, Format, ##__VA_ARGS__).ToPtr()) \


/*-----------------------------------------------------------------------------
    Statically assert defines here to check for malformed configurations.
-----------------------------------------------------------------------------*/

#if !LAL_DO_CHECKS
    #if LAL_DO_SLOW_CHECKS
        #error "Slow checks may not be enabled without normal checks."
    #endif /* LAL_DO_SLOW_CHECKS */
#endif /* !LAL_DO_CHECKS */

#if LAL_DO_SLOW_CHECKS
    #if !LAL_DO_CHECKS
        #error "Slow checks may not be enabled without normal checks."
    #endif /* !LAL_DO_CHECKS */
#endif /* LAL_DO_SLOW_CHECKS */

#if LAL_DO_ENSURES != LAL_DO_CHECKS
    #error "Ensures must be enabled if checks are enabled."
#endif /* LAL_DO_ENSURES != LAL_DO_CHECKS */
