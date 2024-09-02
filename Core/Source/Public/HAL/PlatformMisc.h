// Copyright mzoesch. All rights reserved.

#pragma once

#include "HAL/Platform.h"
#include "HAL/GenericPlatformMisc.h"

#if PLATFORM_WINDOWS
    #include "HAL/PlatformMiscWin.h"
#else /* PLATFORM_WINDOWS */
    /* Add more platform types here in the future... */
    #error "Could not resolve PLATFORM."
#endif /* !PLATFORM_WINDOWS */
