// Copyright mzoesch. All rights reserved.

#pragma once

/*----------------------------------------------------------------------------
    Platform specific includes.
----------------------------------------------------------------------------*/

#if PLATFORM_WINDOWS
    /* Why the duck does this even exists. This destroys so many normal functions - wtf?? */
    #define NOMINMAX

    #include <Windows.h>
#endif /* PLATFORM_WINDOWS */


/*----------------------------------------------------------------------------
    Standard template library.
----------------------------------------------------------------------------*/

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <functional>
#include <memory>
#include <chrono>
