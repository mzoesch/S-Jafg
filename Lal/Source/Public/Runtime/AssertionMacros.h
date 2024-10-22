// Copyright mzoesch. All rights reserved.

#pragma once

/*
 * This is only temporary. We want our own implementation of assertions.
 */
#include <cassert>

#ifndef PRIVATE_INCLUDED_AUTOMATIC_BUILD
    #error "This file must not be included directly. Include Build/Build.h instead."
#endif /* !PRIVATE_INCLUDED_AUTOMATIC_BUILD_PRIVATE */

#define NO_ENTRY_ASSERT_TEXT "Enclosing block should never be entered."

#if DO_CHECKS

    #define checkCode(code)                 do { code; } while ( false );
    #define check(expr)                     CHECK_IMPL_PRIVATE(expr)
    #define checkNoEntry()                  CHECK_IMPL_PRIVATE(false && NO_ENTRY_ASSERT_TEXT)
    #define unimplemented()                 CHECK_IMPL_PRIVATE(false && "Unimplemented code path.")

    // PLATFORM_BREAK() - Currently not using platform break. We of course should be using it.
    // But it is to buggy right now.
    #define CHECK_IMPL_PRIVATE(expr)                       \
        {                                                  \
            if (UNLIKELY(!(expr)))                         \
            {                                              \
                LOG_PRIVATE_UNSAFE_FLUSH_EVERYTHING_FAST() \
                assert(false && #expr);                    \
            }                                              \
        }

#else /* DO_CHECKS */

    #define checkCode(expr)
    #define check(expr)
    #define checkNoEntry()
    #define unimplemented()

#endif /* !DO_CHECKS */

#if DO_SLOW_CHECKS

    #define checkSlow(expr)                 check(expr)

#else /* !DO_SLOW_CHECKS */

    #define checkSlow(expr)

#endif /* !DO_SLOW_CHECKS */

#if DO_ENSURES

    #define ensure(expr)                    check(expr)

#else /* !DO_ENSURES */

    #define ensure(expr)                    expr;

#endif /* !DO_ENSURES */

#if DO_ASSERTS

    #if DO_CHECKS

        #define jassert(expr)               check(expr)
        #define jassertNoEntry()            checkNoEntry()
        #define panic(expr)                 jassert( false && ( expr ) )

    #else /* DO_CHECKS */

        #define jassert(expr)               ASSERT_IMPL_PRIVATE(expr)
        #define jassertNoEntry()            ASSERT_IMPL_PRIVATE(false && NO_ENTRY_ASSERT_TEXT)
        #define panic(expr)                 jassert( false && ( expr ) )

        #define ASSERT_IMPL_PRIVATE(expr)                                 \
            {                                                             \
                if (UNLIKELY(!(expr)))                                    \
                {                                                         \
                    LOG_FATAL(LogCore, "Assertion failed: [{}].", #expr)  \
                }                                                         \
            }

    #endif /* !DO_CHECKS */

#else /* DO_ASSERTS */

    #define jassert(expr)
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
