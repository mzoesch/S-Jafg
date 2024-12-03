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

    typedef LDesktopPlatform LCurrentPlatform;

#elif PLATFORM_WASM

    class LPlatformWasm;
    struct LWasmNativeWindow;
    typedef LWasmNativeWindow LNativeWindow;
    typedef LPlatformWasm LCurrentPlatform;

#else
    #error "Could not resolve PLATFORM."
#endif /* !PLATFORM_WASM */

} /* ~Namespace Jafg */
