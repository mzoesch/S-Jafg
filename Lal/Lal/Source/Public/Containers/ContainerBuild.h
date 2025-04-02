// Copyright mzoesch. All rights reserved.

#pragma once

#if DO_CHECKS
    #define CHECK_CONTAINER_BOUNDS          1
    #define CHECK_STRING_VALIDITY           1
#else /* DO_CHECKS */
    #define CHECK_CONTAINER_BOUNDS          0
    #define CHECK_STRING_VALIDITY           0
#endif /* !DO_CHECKS */
