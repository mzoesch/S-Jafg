// Copyright mzoesch. All rights reserved.

#pragma once

#ifndef LAL_CHECK_CONTAINER_BOUNDS
    #if LAL_DO_CHECKS
        #define LAL_CHECK_CONTAINER_BOUNDS                              1
    #else /* LAL_DO_CHECKS */
        #define LAL_CHECK_CONTAINER_BOUNDS                              0
    #endif /* !LAL_DO_CHECKS */
#endif /* !LAL_CHECK_CONTAINER_BOUNDS */

#ifndef LAL_CHECK_STRING_VALIDITY
    #if LAL_DO_CHECKS
        #define LAL_CHECK_STRING_VALIDITY                               1
    #else /* LAL_DO_CHECKS */
        #define LAL_CHECK_STRING_VALIDITY                               0
    #endif /* !LAL_DO_CHECKS */
#endif /* !LAL_CHECK_STRING_VALIDITY */

#ifndef PRIVATE_LAL_ENSURE_STRING_INVARIANT
    #if LAL_CHECK_STRING_VALIDITY
        #define PRIVATE_LAL_ENSURE_STRING_INVARIANT()                   this->EnsureInvariant();
    #else /* LAL_CHECK_STRING_VALIDITY */
        #define PRIVATE_LAL_ENSURE_STRING_INVARIANT()
    #endif /* !LAL_CHECK_STRING_VALIDITY */
#endif /* PRIVATE_LAL_ENSURE_STRING_INVARIANT */

#ifndef PRIVATE_LAL_ENSURE_STRING_INVARIANT_ON
    #if LAL_CHECK_STRING_VALIDITY
        #define PRIVATE_LAL_ENSURE_STRING_INVARIANT_ON(What)            (What).EnsureInvariant();
    #else /* LAL_CHECK_STRING_VALIDITY */
        #define PRIVATE_LAL_ENSURE_STRING_INVARIANT_ON(What)
    #endif /* !LAL_CHECK_STRING_VALIDITY */
#endif /* PRIVATE_LAL_ENSURE_STRING_INVARIANT_ON */

#ifndef PRIVATE_LAL_WITH_LEGACY_ALLOCATORS
    #define PRIVATE_LAL_WITH_LEGACY_ALLOCATORS                          0
#endif /* PRIVATE_LAL_WITH_LEGACY_ALLOCATORS */
