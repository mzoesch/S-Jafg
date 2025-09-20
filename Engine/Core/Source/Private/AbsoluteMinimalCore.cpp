// Copyright mzoesch. All rights reserved.

#if PLATFORM_LINUX
    #ifndef LAL_PLATFORM_CALLSPEC_IN
        #define LAL_PLATFORM_CALLSPEC_IN                                __attribute__ ((visibility ("default")))
    #endif /* !LAL_PLATFORM_CALLSPEC_IN */
    #ifndef LAL_PLATFORM_CALLSPEC_OUT
        #define LAL_PLATFORM_CALLSPEC_OUT                               __attribute__ ((visibility ("default")))
    #endif /* !LAL_PLATFORM_CALLSPEC_OUT */
#elif PLATFORM_WINDOWS
    #ifndef LAL_PLATFORM_CALLSPEC_IN
        #define LAL_PLATFORM_CALLSPEC_IN                                __declspec ( dllimport )
    #endif /* !LAL_PLATFORM_CALLSPEC_IN */
    #ifndef LAL_PLATFORM_CALLSPEC_OUT
        #define LAL_PLATFORM_CALLSPEC_OUT                               __declspec ( dllexport )
    #endif /* !LAL_PLATFORM_CALLSPEC_OUT */
#else /* PLATFORM_WINDOWS */
    #error "Missing implementation for this PLATFORM."
#endif /* PLATFORM_WINDOWS */

namespace JafgCore
{

CORE_API bool bGAlreadyCrashed          { false };
CORE_API bool bGSuppressCrashDialog     { false };

} /* ~Namespace JafgCore */
