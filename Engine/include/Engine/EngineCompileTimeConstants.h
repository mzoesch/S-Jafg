// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

#ifndef JAFG_WITH_FOREIGN_SUPPORT
    #if JAFG_PLATFORM_SUPPORTS_SHARED_LIBRARIES
        #define JAFG_WITH_FOREIGN_SUPPORT                               1
    #else /* JAFG_PLATFORM_SUPPORTS_SHARED_LIBRARIES */
        #define JAFG_WITH_FOREIGN_SUPPORT                               1
    #endif /* JAFG_PLATFORM_SUPPORTS_SHARED_LIBRARIES */
#endif /* JAFG_WITH_FOREIGN_SUPPORT */

#if !JAFG_PLATFORM_SUPPORTS_SHARED_LIBRARIES
    #if JAFG_WITH_FOREIGN_SUPPORT
        #error "Illformed JAFG_WITH_FOREIGN_SUPPORT. This is only supported on platforms that support shared libraries."
    #endif /* JAFG_WITH_FOREIGN_SUPPORT */
#endif /* !JAFG_PLATFORM_SUPPORTS_SHARED_LIBRARIES */
