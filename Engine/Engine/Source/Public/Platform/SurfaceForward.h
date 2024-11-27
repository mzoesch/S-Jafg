// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

class LViewport;
class LSurface;

#if PLATFORM_DESKTOP

    class LDesktopPlatformBase;

    struct LDesktopSurfaceProps;
    struct LNativeDesktopWindowBase;

    #if PLATFORM_WINDOWS
        class LDesktopPlatformWin;
        struct LNativeWindowWin;
        typedef LNativeWindowWin LNativeWindow;
        /** The currently active desktop platform. */
        typedef LDesktopPlatformWin LDesktopPlatform;
    #endif /* PLATFORM_WINDOWS */

#endif /* PLATFORM_DESKTOP */

} /* ~Namespace Jafg */
