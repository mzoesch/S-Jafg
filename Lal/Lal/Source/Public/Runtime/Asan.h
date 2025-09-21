// Copyright mzoesch. All rights reserved.

#pragma once

#if LAL_WITH_SANITIZERS
    #if LAL_WITH_CLANG
        #ifndef LAL_ASAN_NO_UB
            #define LAL_ASAN_NO_UB                                      __attribute__ ((no_sanitize("undefined")))
        #endif /* !LAL_ASAN_NO_UB */
        #ifndef LAL_ASAN_NO_NULL_UB
            #define LAL_ASAN_NO_NULL_UB                                 __attribute__ ((no_sanitize("null")))
        #endif /* !LAL_ASAN_NO_NULL_UB */
    #endif /* LAL_WITH_CLANG */
#endif /* !LAL_WITH_SANITIZERS */

#ifndef LAL_ASAN_NO_UB
    #define LAL_ASAN_NO_UB
#endif /* !LAL_ASAN_NO_UB */

#ifndef LAL_ASAN_NO_NULL_UB
    #define LAL_ASAN_NO_NULL_UB
#endif /* !LAL_ASAN_NO_NULL_UB */
