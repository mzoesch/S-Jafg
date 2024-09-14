// Copyright mzoesch. All rights reserved.

#pragma once

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

#if DO_ENSURES

    #define ensure(expr)                    check(expr)

#else /* !DO_ENSURES */

    #define ensure(expr)                    expr;

#endif /* !DO_ENSURES */

#if DO_ASSERTS

    #if DO_CHECKS

        #define jassert(expr)               check(expr)
        #define jassertNoEntry()            checkNoEntry()

    #else /* DO_CHECKS */

        #define jassert(expr)               ASSERT_IMPL_PRIVATE(expr)
        #define jassertNoEntry()            ASSERT_IMPL_PRIVATE(false && NO_ENTRY_ASSERT_TEXT)

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

#endif /* !DO_ASSERTS */
