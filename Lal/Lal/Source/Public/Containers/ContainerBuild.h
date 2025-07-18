// Copyright mzoesch. All rights reserved.

#pragma once

#ifndef LAL_CHECK_CONTAINER_BOUNDS
    #if LAL_DO_CHECKS
        #define LAL_CHECK_CONTAINER_BOUNDS          1
    #else /* LAL_DO_CHECKS */
        #define LAL_CHECK_CONTAINER_BOUNDS          0
    #endif /* !LAL_DO_CHECKS */
#endif /* !LAL_CHECK_CONTAINER_BOUNDS */

#ifndef LAL_CHECK_STRING_VALIDITY
    #if LAL_DO_CHECKS
        #define LAL_CHECK_STRING_VALIDITY           1
    #else /* LAL_DO_CHECKS */
        #define LAL_CHECK_STRING_VALIDITY           0
    #endif /* !LAL_DO_CHECKS */
#endif /* !LAL_CHECK_STRING_VALIDITY */
