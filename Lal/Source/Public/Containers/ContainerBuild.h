// Copyright mzoesch. All rights reserved.

#pragma once

#if IN_DEBUG || IN_DEVELOPMENT
    #define CHECK_CONTAINER_BOUNDS 1
#else /* IN_DEBUG || IN_DEVELOPMENT */
    #define CHECK_CONTAINER_BOUNDS 0
#endif /* !(IN_DEBUG || IN_DEVELOPMENT) */
