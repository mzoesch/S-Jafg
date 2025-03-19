// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg::TesterForward
{

#ifdef PLATFORM_WASM
    int DontDiscardMe();
#elif PLATFORM_WINDOWS
    __declspec( dllexport ) int DontDiscardMe();
#else /* PLATFORM_WINDOWS */
    int DontDiscardMe();
#endif /* !PLATFORM_WINDOWS */

} /* ~Namespace Jafg::TesterForward */
