// Copyright mzoesch. All rights reserved.

#pragma once

#if LAL_DO_CHECKS
    #define CHECK_CONTAINER_BOUNDS          1
    #define CHECK_STRING_VALIDITY           1
#else /* LAL_DO_CHECKS */
    #define CHECK_CONTAINER_BOUNDS          0
    #define CHECK_STRING_VALIDITY           0
#endif /* !LAL_DO_CHECKS */
