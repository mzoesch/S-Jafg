// Copyright mzoesch. All rights reserved.

#pragma once

#if JAFG_WITH_SANITIZERS
    #if JAFG_WITH_CLANG
        #ifndef JAFG_ASAN_NO_UB
            #define JAFG_ASAN_NO_UB                                     __attribute__ ((no_sanitize("undefined")))
        #endif /* !JAFG_ASAN_NO_UB */
        #ifndef JAFG_ASAN_NO_NULL_UB
            #define JAFG_ASAN_NO_NULL_UB                                __attribute__ ((no_sanitize("null")))
        #endif /* !JAFG_ASAN_NO_NULL_UB */
    #endif /* JAFG_WITH_CLANG */
#endif /* !JAFG_WITH_SANITIZERS */

#ifndef JAFG_ASAN_NO_UB
    #define JAFG_ASAN_NO_UB
#endif /* !JAFG_ASAN_NO_UB */

#ifndef JAFG_ASAN_NO_NULL_UB
    #define JAFG_ASAN_NO_NULL_UB
#endif /* !JAFG_ASAN_NO_NULL_UB */
